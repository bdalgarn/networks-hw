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
#include "ps_client/client.h"
#include "ps_client/callback.h"
#include "ps_client/thread.h"
#include "ps_client/queue.h"

// the client constructor initializes the private variables and the locks
Client::Client(const char *host, const char *port, const char *cid){
  this->host = host;
  this->port = port;
  this->cid = cid;
  this->shutdownVal = false;
  //  this->clientNonce = rand() % 200 + 1;

  pthread_mutex_init(&(this->bool_lock1), NULL);
  pthread_mutex_init(&(this->bool_lock2), NULL);

}

Client::~Client() {}


// the publish function creates publish messages to send to the server
void Client::publish(const char * topic, const char * message, size_t length){
  Message m;
  std::string top(topic); // converts char * topic to string
  std::string bod(message, length); // converts char * message to string
  // sets all of the members of the message object
  m.type = "PUBLISH";
  m.topic = top;
  m.body = bod;
  m.sender = cid;
  this->outgoing.push(m); // pushes the publish message to the outgoing queue

}


// the run function starts and joins the sender and receiver threads
void Client::run(){
  Thread thread1;

  thread1.start(receiver,this);
  thread1.detach();

  std::map<std::string,Callback *>::iterator it;
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
    std::cerr << "fprintf failed" << std::endl;
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



/* vim: set expandtab sts=4 sw=4 ts=8 ft=cpp: */
