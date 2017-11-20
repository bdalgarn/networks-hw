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
#include "thread.h"
#include "queue.h"


int main(int argc, char * argv[]){

  if (argc != 4) exit(1);

  const char *host      = argv[1];
  const char *port      = argv[2];
  const char *client_id = argv[3];

  Client client(host, port, client_id);

  FILE *server_file = client.socket_connect(client.getHost(), client.getPort());

  if (server_file == NULL) {
    fprintf(stderr, "ERROR: Unable to connect to %s:%s: %s", client.getHost(), client.getPort(), strerror(errno));
  }

  char buffer[BUFSIZ];
  sprintf(buffer, "%s", client_id);
  client.sendToServer(server_file, buffer);
  bzero(buffer, BUFSIZ);
  client.recvFromServer(server_file, buffer);
  fprintf(stdout, "%s", buffer);
  if (fgets(buffer, BUFSIZ, stdin) == NULL){
    fprintf(stderr, "Failed to recieve response\n");
    exit(1);
  }
  // Send password
  //client.sendToServer(server_file, buffer);
  char recBuffer[BUFSIZ];
  send(client.getFd(), (void *)buffer, BUFSIZ, 0); 
  bzero(recBuffer, BUFSIZ);
  //client.recvFromServer(server_file, recBuffer);
  recv(client.getFd(), (void *)recBuffer, BUFSIZ, 0); 
  if (strcmp(recBuffer, "ACK_REG") == 0) {
	printf("Successfully registered\n");
  }
  else {
    while(strcmp(recBuffer, "Invalid Password.") == 0){
      bzero(recBuffer, BUFSIZ);
      fprintf(stdout, "Try Again, wrong password:\n");
      fgets(buffer, BUFSIZ, stdin);
      send(client.getFd(), (void *)buffer, BUFSIZ, 0); 
      recv(client.getFd(), (void *)recBuffer, BUFSIZ, 0); 
    } 
    // Receive ack
    bzero(recBuffer, BUFSIZ);
    //recv(client.getFd(), (void *)recBuffer, BUFSIZ, 0); 
    printf("%s\n", recBuffer);
  }
  client.run();

  char user_id[BUFSIZ];
  char message[BUFSIZ];

  bool operationCompleted = false;
  char incBuffer[BUFSIZ];
  while (!client.shutdown()){
     printf("Choose an Operation:\n\tB: Message Broadcasting\n\tP: Private Messaging\n\tE: Exit\n");
     bzero(buffer, BUFSIZ);
     fgets(buffer, BUFSIZ, stdin);
     //client.sendToServer(server_file, buffer);
     send(client.getFd(), (void *)buffer, BUFSIZ, 0); 
     

     if(strcmp(buffer,"B") == 0){
       while(!operationCompleted){
	 fprintf(stdout,"Enter Message to send:\n");
	 sprintf(incBuffer, "%s", client.incoming.pop());
	 if (incBuffer[1] == 'C') {
	   sscanf(incBuffer, "C,%s,%s", user_id, message);
	   fprintf(stdout, "### New Message: Message recieved from %s: %s ###", user_id, message);
	   continue;
	  }
	 else if(incBuffer[1] == 'P'){
	    fgets(buffer, BUFSIZ, stdin);
	    client.sendToServer(server_file, buffer);
	    client.recvFromServer(server_file, recBuffer);
	    fprintf(stdout, recBuffer);
	    operationCompleted = true;
	 }
       }
     }      
      else if (strcmp(buffer,"P") == 0){
	 
 	  while(!operationCompleted){
	    sprintf(incBuffer, "%s", client.incoming.pop());
	    if (incBuffer[1] == 'C') {
	      sscanf(incBuffer, "C,%s,%s", user_id, message);
	      fprintf(stdout, "### New Message: Message recieved from %s: %s ###", user_id, message);
	       continue;
	    }
	    else if (incBuffer[1] == 'L'){
	      char list[BUFSIZ];
	      sscanf(incBuffer, "L, %s", list);
	      fprintf(stdout, "Users online: %s", list);
	      break;
	    }
	  }

	  while(!operationCompleted){
	    fprintf(stdout,"Which online user would you like to send to?\n");
	    sprintf(incBuffer, "%s", client.incoming.pop());
	    if (incBuffer[1] == 'C') {
	      sscanf(incBuffer, "C,%s,%s", user_id, message);
              fprintf(stdout, "### New Message: Message recieved from %s: %s ###", user_id, message);
	      continue;
	    }
	     else if(incBuffer[1] == 'P'){
		fgets(buffer, BUFSIZ, stdin);
		client.sendToServer(server_file, buffer);
		break;
	     }
	  }

	  while(!operationCompleted){
	    fprintf(stdout,"Message\n");
	    sprintf(incBuffer, "%s", client.incoming.pop());
	    if (incBuffer[1] == 'C') {
	      sscanf(incBuffer, "C,%s,%s", user_id, message);
              fprintf(stdout, "### New Message: Message recieved from %s: %s ###", user_id, message);
	      continue;
	     }
	     else if(incBuffer[1] == 'P'){
		fgets(buffer, BUFSIZ, stdin);
		client.sendToServer(server_file, buffer);
		client.recvFromServer(server_file, recBuffer);
		fprintf(stdout,recBuffer);
		operationCompleted = true;
	    }
	  }
      }
	else if (strncmp(buffer,"E", 1) == 0){
	  client.shutdownVal = true;
	}
	else {
	  fprintf(stdout,"Invalid operation.\n\n Choose an Operation:\n\tB: Message Broadcasting\n\tP: Private Messaging\n\tE: Exit\n");
	}
     operationCompleted = false;
  }

  fclose(server_file); // close the socket connection
  return 0;
}

