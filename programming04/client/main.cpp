/* main.cpp
Brianna Hoelting bhoeltin
Anthony DiFalco adifalco 
Ben Dalgarn bdalgarn 

This is the main of the chat client. It includes the client and thread classes and instanstiates a connection with the server. This file will aslo ensure that another thread is created in order to not block other operations when a message is recieved. When a client is created, it is first ensured that the user exists, if not, they are registered with a new password. If they exist, the program will ask them for a password to verify that they are that user. This file also parses and handles the messages from the server. It will take in client input for the operation that they want to complete and then will complete a series of actions based on that operation. The operation type will be sent to the server and depending on which operation, the server will send back a message. For broadcasting the server will send back a messag ethat asks for the message to be sent. The client will then recieve a confirmation, print it out, and return to prompt state. If the user wants Private message mode, the server will send back a list of users that are online and then ask the user whih user they want to send to. This is sent to the server which then asks for the message that they want to send. AS with broadcasting, a confirmation from the server is recieved, printed out, and returns to prompt user for operation. If the user puts in an E, then the client is taken offline. 

*/

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
  sprintf(buffer, "%s\n", client_id);
  client.sendToServer(server_file, buffer);

  client.recvFromServer(server_file, buffer);
  fprintf(stdout, "%s\n", buffer);
  if (fgets(buffer, BUFSIZ, stdin) == NULL){
    fprintf(stderr, "Failed to recieve response\n");
    exit(1);
  }
  client.sendToServer(server_file, buffer);
  char recBuffer[BUFSIZ];
  client.recvFromServer(server_file, recBuffer);
  while(strcmp(recBuffer, "Invalid Password.")){
    fprintf(stdout, "Try Again, wrong password:\n");
    fgets(buffer, BUFSIZ, stdin);
    client.sendToServer(server_file, buffer);
    client.recvFromServer(server_file, recBuffer);
  } 

  client.run();

  char user_id[BUFSIZ];
  char message[BUFSIZ];

  bool operationCompleted = false;
  char incBuffer[BUFSIZ];
  while (!client.shutdown()){
     
    // prompt for operation mode
    fprintf(stdout,"Choose an Operation:\n\tB: Message Broadcasting\n\tP: Private Messaging\n\tE: Exit\n");
     bzero(buffer, BUFSIZ);
     fgets(buffer, BUFSIZ, stdin);
     client.sendToServer(server_file, buffer);
     
     // user wants to broadcast
     if(strcmp(buffer,"B") == 0){
       //this will allow the program to see if any interrupting messages are recieved
       while(!operationCompleted){ 
	 fprintf(stdout,"Enter Message to send:\n");
	 sprintf(incBuffer, "%s", client.incoming.pop());
	 // if the message in the incoming buffer is a message recieved by the server
	 if (incBuffer[1] == 'C') {
	   sscanf(incBuffer, "C,%s,%s", user_id, message);
	   fprintf(stdout, "### New Message: Message recieved from %s: %s ###", user_id, message);
	   continue;
	  }
	 // if the message in the incoming buffer is an action to be taken by the client
	 else if(incBuffer[1] == 'P'){
	    fgets(buffer, BUFSIZ, stdin);
	    client.sendToServer(server_file, buffer);
	    //confirmation message
	    client.recvFromServer(server_file, recBuffer);
	    fprintf(stdout, recBuffer);
	    operationCompleted = true;
	 }
       }
     }      
     // user wants private mode
      else if (strcmp(buffer,"P") == 0){
	//while loops below ensure that interrupting messages are printed and do not disturb prompting of user
 	  while(!operationCompleted){
	    sprintf(incBuffer, "%s", client.incoming.pop());
	    // content message was recieved
	    if (incBuffer[1] == 'C') {
	      sscanf(incBuffer, "C,%s,%s", user_id, message);
	      fprintf(stdout, "### New Message: Message recieved from %s: %s ###", user_id, message);
	       continue;
	    }
	    // this is if the server sends a list of users
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
	    // server prompts client for user to send message to
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
	    // server prompts user for message to send
	     else if(incBuffer[1] == 'P'){
		fgets(buffer, BUFSIZ, stdin);
		client.sendToServer(server_file, buffer);
		client.recvFromServer(server_file, recBuffer);
		fprintf(stdout,recBuffer);
		operationCompleted = true;
	    }
	  }
      }

     // user wants to exit
	else if (strcmp(buffer,"E") == 0){
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

