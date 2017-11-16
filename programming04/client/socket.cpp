#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>
#include <iostream>
#include <ps_client/client.h>
#include <ps_client/thread.h>
#include <ps_client/callback.h>
#include <ps_client/queue.h>

FILE * socket_connect(const char *host, const char *port){
  struct addrinfo *results;

  struct addrinfo hints = {};
  hints.ai_family = AF_UNSPEC;     /* Return IPv4 and IPv6 choices */
  hints.ai_socktype = SOCK_STREAM; /* Use TCP */

  int status;
  if((status = getaddrinfo(host, port, &hints, &results)) != 0){
    fprintf(stderr, "getaddrinfo failed: %s\n", gai_strerror(status));
    return NULL;
  }

  int client_fd = -1;
  for(struct addrinfo *p = results; p != NULL && client_fd < 0; p=p->ai_next){
    /* Allocate socket */
    if((client_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0){
      fprintf(stderr, "Unable to make socket: %s\n", strerror(errno));
      continue;
    }

    /* Connect to host */
    if(connect(client_fd, p->ai_addr, p->ai_addrlen) < 0){
      close(client_fd);
      client_fd = -1;
      continue;
    }
  }

  /* Release allocate address information */
  freeaddrinfo(results);

  if(client_fd < 0){
    return NULL;
  }

  /* Open file stream from socket file descriptor */
  FILE * client_file = fdopen(client_fd, "w+");
  if(client_file == NULL){
    fprintf(stderr, "Unable to fdopen: %s\n", strerror(errno));
    close(client_fd);
    return NULL;
  }


  return client_file;
}

void * sender(void * arg) {
  Client * client = (Client * ) arg;

  FILE *server_file = socket_connect(client->getHost(), client->getPort());
  if (server_file == NULL) {
    fprintf(stderr, "ERROR: Unable to connect to %s:%s: %s", client->getHost(), client->getPort(), strerror(errno));
  }

  client->identify(server_file);

  while(!client->shutdown()){
    Message m = (client->outgoing).pop();
    char buffer[BUFSIZ];
    if(m.type == "SUBSCRIBE"){
      sprintf(buffer, "SUBSCRIBE %s\n", (m.topic).c_str());
      client->sendToServer(server_file, buffer);
    }
    else if(m.type == "UNSUBSCRIBE"){
      sprintf(buffer, "UNSUBSCRIBE %s\n", (m.topic).c_str());
      client->sendToServer(server_file, buffer);
    }
    else if(m.type == "PUBLISH"){
      char body[BUFSIZ];
      sprintf(body, "%s", (m.body).c_str());
      int body_length = strlen(body);
      sprintf(buffer, "PUBLISH %s %hi\n%s", (m.topic).c_str(), body_length, (m.body).c_str());
      client->sendToServer(server_file, buffer);
    }
    else if(m.type == "DISCONNECT"){
      sprintf(buffer, "DISCONNECT %s %hi\n", (m.sender).c_str(), (int)m.nonce);
      client->sendToServer(server_file, buffer);
    }
    char recvBuffer[BUFSIZ];
    char returnBuffer[BUFSIZ];
    sprintf(returnBuffer,"%s",client->recvFromServer(server_file, recvBuffer));
    client->serverResponse = client->checkServerResponse(returnBuffer);
  }

  int * p = new int;
  void * retval = p;
  fclose(server_file);
  delete p;
  return retval;
}

void * receiver(void * arg) {
  Client * client = (Client * ) arg;

  FILE *server_file = socket_connect(client->getHost(), client->getPort());
  if (server_file == NULL) {
    std::cerr << "Socket connection failed" << std::endl;
    exit(1);
  }

  client->identify(server_file);

  char message[BUFSIZ];
  sprintf(message, "RETRIEVE %s\n", client->getCid());

  while(!client->shutdown()){
    Message m;
    char returnBuffer[BUFSIZ];
    char topic[BUFSIZ];
    char sender[BUFSIZ];
    int length;
    std::string response;

    client->sendToServer(server_file, message);

    if (fgets(returnBuffer, BUFSIZ, server_file) != NULL){
      sscanf(returnBuffer, "MESSAGE %s FROM %s LENGTH %d", topic, sender, &length);
      m.type = "RETRIEVE";
	    m.topic = std::string(topic);
	    m.sender = std::string(sender);
	    m.nonce = client->getNonce();
	  }

    if(fgets(returnBuffer, length + 1, server_file) != NULL)
	  m.body = returnBuffer;
    (client->incoming).push(m);
  }

  int * p = new int;
  void * retval = p;
  fclose(server_file);
  delete p;
  return retval;
}

/* vim: set expandtab sts=4 sw=4 ts=8 ft=cpp: */
