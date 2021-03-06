// thread.cpp
/*                   
Brianna Hoelting bhoeltin
Anthony DiFalco adifalco 
Ben Dalgarn bdalgarn 


This is the implmentation of the thread class. It uses pthread to create and maintian the threads. 


*/

#include <iostream>
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
#include "thread.h"
#include <pthread.h>

void Thread::start (thread_func func, void * arg){
  if (pthread_create(&p, NULL, func, arg) != 0){
    std::cerr << "Error in thread creation" << std::endl;
    exit(1);
  }
}

void Thread::join (void ** result){
  pthread_join(p, result);
}

void Thread::detach(){
  if((pthread_detach(p)) != 0) {
    std::cerr << "Error in detachment" << std::endl;
    exit(1);
  }
}
