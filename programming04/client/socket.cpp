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
#include "client.h"
#include "thread.h"
#include "callback.h"
#include "queue.h"

// function that connects to the socket
FILE * socket_connect(const char *host, const char *port){
  struct addrinfo *results;

  struct addrinfo hints = {};
  hints.ai_family = AF_UNSPEC;     /* Return IPv4 and IPv6 choices */
  hints.ai_socktype = SOCK_STREAM; /* Use TCP */

  int status;
  if((status = getaddrinfo(host, port, &hints, &results)) != 0){ // gets the address info using the socket information
    fprintf(stderr, "getaddrinfo failed: %s\n", gai_strerror(status)); // outputs an error message if getaddrinfo failed
    return NULL;
  }

  int client_fd = -1;
  for(struct addrinfo *p = results; p != NULL && client_fd < 0; p=p->ai_next){ // for loop that goes through the results of addrinfo and creates sockets
    if((client_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0){ // allocates the socket
      fprintf(stderr, "Unable to make socket: %s\n", strerror(errno)); // outputs an error message if could ot create socket
      continue;
    }

    /* Connect to host */
    if(connect(client_fd, p->ai_addr, p->ai_addrlen) < 0){ // connects the socket
      close(client_fd); // if the socket doesn't connect close it
      client_fd = -1;
      continue;
    }
  }

  /* Release allocate address information */
  freeaddrinfo(results);

  // make sure not to try and oppen the client file descriptor if it is less than zero
  if(client_fd < 0){
    return NULL;
  }

  /* Open file stream from socket file descriptor */
  FILE * client_file = fdopen(client_fd, "w+");
  if(client_file == NULL){ // outputs an error message if could not open the client file
    fprintf(stderr, "Unable to fdopen: %s\n", strerror(errno));
    close(client_fd);
    return NULL;
  }


  return client_file;
}


// function that creates the functionality of the receiver thread
void * receiver(void * arg) {
  Client * client = (Client * ) arg; // creates a Client object

  FILE *server_file = socket_connect(client->getHost(), client->getPort()); // calls socket_connect to create the socket
  // if server_file is NULL then the socket connection didn't work and an error message is printed
  if (server_file == NULL) {
    std::cerr << "Socket connection failed" << std::endl;
    exit(1);
  }

  //client->identify(server_file); // call the identify function

  char message[BUFSIZ];
  sprintf(message, "RETRIEVE %s\n", client->getCid()); // creates the beginning of the RETRIEVE message

  // while loop that runs as long as the client isn't supposed to shut down
  while(!client->shutdown()){
    Message m;
    char returnBuffer[BUFSIZ];
    char topic[BUFSIZ];
    char sender[BUFSIZ];
    int length;
    std::string response;

    client->sendToServer(server_file, message); // sends a message to the server

    // gets the response from the server and creates a RETRIEVE message if there is a message to receive
    if (fgets(returnBuffer, BUFSIZ, server_file) != NULL){
      sscanf(returnBuffer, "MESSAGE %s FROM %s LENGTH %d", topic, sender, &length);
      m.type = "RETRIEVE";
	    m.topic = std::string(topic);
	    m.sender = std::string(sender);
	  }

    // sets the body of the message from the server
    if(fgets(returnBuffer, length + 1, server_file) != NULL){
      m.body = returnBuffer;
    }

    (client->incoming).push(m); // adds the message from the server to the incoming queue
  }

  int * p = new int; // use this to set the value of the void *
  void * retval = p; // have to return a void * from the function
  fclose(server_file); // close the socket connection
  delete p; // need to delete the int that was allocated
  return retval;
}

/* vim: set expandtab sts=4 sw=4 ts=8 ft=cpp: */
