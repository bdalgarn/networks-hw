/*
Brianna Hoelting bhoeltin
Anthony DiFalco adifalco
Ben Dalgarn bdalgarn


This is the implementaiton of the Client class. It contains all the functions necessary to connect and communicated to the client to the server and the function that will be used by another thread to recieve messages from the server. The reciever function recieves from the server and adds to an incoming queue that will be popped by the client in main. The Client class is instantiated in main.


 */
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
#include <queue>
#include <errno.h>
#include <map>
#include <vector>
#include <iostream>
#include "client.h"
#include "thread.h"
#include "queue.h"

// the client constructor initializes the private variables and the locks
Client::Client(const char *host, const char *port, const char *cid){
  this->host = host;
  this->port = port;
  this->cid = cid;
  this->shutdownVal = false;
  //  this->clientNonce = rand() % 200 + 1;

  pthread_mutex_init(&(this->bool_lock1), NULL);
  pthread_mutex_init(&(this->bool_lock2), NULL);
  this->client_fd = 0;
}

Client::~Client() {}

int Client::getFd() {
    return this->client_fd;
}


// the run function starts and joins the sender and receiver threads
void Client::run(){
  Thread thread1;

  thread1.start(receiver,this);
  thread1.detach();

}
 // the shutdown function simply returns the value of shutdownVal
bool Client::shutdown(){
  int rc;
  // this code must be locked because shutdownVal can be read and written at the same time
  Pthread_mutex_lock(&(this->bool_lock1));
  bool val = shutdownVal;
  Pthread_mutex_unlock(&(this->bool_lock1));
  return val;
}

// this function uses fprintf to send a specified buffer to the server across the socket
void Client::sendToServer(FILE * socketnum, char * buffer){
  // if fprintf returns an error display this and exit
  if (fprintf(socketnum, buffer) < 0) {
    //std::cerr << "fprintf failed:" << strerror(errno) << std::endl;
    printf("fprintf failed\n");
    exit(1);
  }
}

// this function uses fgets to receive a buffer from the server across the socket
char * Client::recvFromServer(FILE * socketnum, char * buffer){
  if (fgets(buffer, BUFSIZ, socketnum) != NULL){ // if fgets works then return the buffer that was received
    return buffer;
  } else { // if fgets returns an error display this and exit
    std::cerr << "Failed to recieve response from server\n" << std::endl;
    exit(1);
  }
}

const char * Client::getHost(){
  return host;
}

// function that returns the port value                                                                                                                                                                                                       
const char * Client::getPort(){
  return port;
}

// function that returns the cid value                                                                                                                                                                                                        
const char * Client::getCid(){
  return cid;
}


FILE * Client::socket_connect(const char *host, const char *port){
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
 this->client_fd = client_fd;
 return fdopen(client_fd, "r+");
}
}
 void * receiver(void * arg) {
    Client * client = (Client * ) arg; // creates a Client object                                                       

    FILE *server_file = client->socket_connect(client->getHost(), client->getPort()); // calls socket_connect to create the socket                                                                                                                   
  // if server_file is NULL then the socket connection didn't work and an error message is printed                    
  if (server_file == NULL) {
    std::cerr << "Socket connection failed" << std::endl;
    exit(1);
  }

//client->identify(server_file); // call the identify function                                                      


// while loop that runs as long as the client isn't supposed to shut down                                           
 while(!client->shutdown()){

   char returnBuffer[BUFSIZ];
   // gets the response from the server and creates a RETRIEVE message if there is a message to receive              
   if (fgets(returnBuffer, BUFSIZ, server_file) != NULL){
     client->incoming.push(returnBuffer);
   }
 }

 int * p = new int; // use this to set the value of the void *                                                       
 void * retval = p; // have to return a void * from the function                                                     
 fclose(server_file); // close the socket connection                                                                 
 delete p; // need to delete the int that was allocated                                                              
 return retval;
  }



/* vim: set expandtab sts=4 sw=4 ts=8 ft=cpp: */
