// client.h: PS Client Library -------------------------------------------------
/*                   
Brianna Hoelting bhoeltin 
Anthony DiFalco adifalco  
Ben Dalgarn bdalgarn 

This is the header file for the client class. It has many public functions that enable it to communicate with the server as well as connect to the server. Additionally it has many private variables that keep track of important information for communication and message parsing/recieving. 


*/


#pragma once

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
#include <map>
#include "queue.h"
#include <vector>

// the functions that are used to implement the publish and retrieve threads
void * sender(void *);
void * receiver(void *);

class Client {
    public:
        Client(const char *, const char *, const char *); // Client class constructor
        ~Client(); // Client class destructor
        void run(); // function that creates the threads passing in the sender and receiver functions
        bool shutdown(); // function that shuts the program down
        void sendToServer(FILE *, char *); // function that sends a message to the server
        char * recvFromServer(FILE *, char *); // function that receives a message from the server
        bool serverResponse; // value that is used to store the response from the server
        pthread_mutex_t bool_lock1, bool_lock2; // mutex locks used for synchronization
	const char * getHost(); // function that returns the host 
        const char * getPort(); // function that returns the port 
        const char * getCid(); // function that returns the client ID
	FILE * socket_connect(const char *, const char *);
	Queue<char *> incoming;
	bool shutdownVal;
        int getFd();
        int client_fd;

    private:
        std::string user_id; // value that stores the name of the user
        const char * host; // value that stores the host number
        const char * port; // value that stores the port number
        const char * cid; // value that stores the client's ID
};

// vim: set expandtab sts=4 sw=4 ts=8 ft=cpp: ----------------------------------
