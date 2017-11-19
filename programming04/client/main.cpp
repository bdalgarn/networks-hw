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
#include <queue>
#include <errno.h>
#include <map>
#include <vector>
#include <iostream>
#include "client.h"
#include "callback.h"
#include "thread.h"
#include "queue.h"


int main(int argc, char * argv[]){

  if (argc != 4) exit(1);

  const char *host      = argv[1];
  const char *port      = argv[2];
  const char *client_id = argv[3];

  Client client(host, port, client_id);

  FILE *server_file = socket_connect(client->getHost(), client->getPort());

  if (server_file == NULL) {
    fprintf(stderr, "ERROR: Unable to connect to %s:%s: %s", client->getHost(), client->getPort(), strerror(errno));
  }

  char buffer[BUFSIZ];
  sprintf(buffer, "%s\n", client_id);
  client.sendToServer(server_file, buffer);

  fprintf(stdout, "Please enter your password:\n");
  if (fgets(buffer, BUFSIZ, stdin) == NULL){
    fprintf(stderr, "Failed to recieve response\n");
    exit(1);
  }
  client.sendToServer(server_file, buffer);
  char recBuffer[BUFSIZ];
  client.recFromServer(server_file, recbuffer);
  while(!(recBuffer == "Correct")){
    fprintf(stdout, "Incorrect password\n");
    exit(1);
  } 

  client.run();

  while (!client.shutdown()){

    fprintf(stdout,"Choose an Operation:\n\tB: Message Broadcasting\n\tP: Private Messaging\n\tE: Exit\n");
    bzero(buffer, BUFSIZ);
    fgets(buffer, BUFSIZ, stdin);
    client.sendToServer(server_file, buffer);
    if(buffer == "B" || buffer == "b"){
      client.recFromServer(server_file, recbuffer);
      if( recBuffer == "prompt"){
	fprintf(stdout,"Enter Message to send:\n");
	fgets(buffer, BUFSIZ, stdin);
	client.sendToServer(server_file, buffer);
	client.recFromServer(server_file, recBuffer);
	fprintf(stdout, recBuffer);
      }

    }
    else if (buffer == "P" || buffer == "p"){
      client.recFromServer(server_file, recbuffer);
      if( recBuffer != NULL){
	fprintf(stdout,"Which online user would you like to send to?\n");
	fgets(buffer, BUFSIZ, stdin);
	fprintf(stdout,"Message:\n");
	fgets(recBuffer, BUFSIZ, stdin);
	char newBuffer[BUFSIZ];
	sprintf(newBuffer, "%s %s", buffer, recBuffer);
	client.sendToServer(server_file, newBuffer);
	client.recFromServer(server_file, recBuffer);
	fprintf(stdout,recBuffer);
    }
    else if (buffer == "E" || buffer == "e"){
      client.shutdown = true;
    }
    else {
      fprintf(stdout,"Invalid operation.\n\n Choose an Operation:\n\tB: Message Broadcasting\n\tP: Private Messaging\n\tE: Exit\n");
    }

  }

  return 0;

}
