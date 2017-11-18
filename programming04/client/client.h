// client.h: PS Client Library -------------------------------------------------

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
#include "callback.h"
#include <vector>

// the functions that are used to implement the publish and retrieve threads
void * sender(void *);
void * receiver(void *);

class Client {
    public:
        Client(const char *, const char *, const char *); // Client class constructor
        ~Client(); // Client class destructor
        void publish(const char *, const char *, size_t); // function that publishes to server
        void run(); // function that creates the threads passing in the sender and receiver functions
        bool shutdown(); // function that shuts the program down
        void sendToServer(FILE *, char *); // function that sends a message to the server
        char * recvFromServer(FILE *, char *); // function that receives a message from the server
        Queue<Message> incoming; // queue that stores incoming messages from server when doing retrieve
        Queue<Message> outgoing; // queue that stores outgoing messages to be sent to server
        bool serverResponse; // value that is used to store the response from the server
        pthread_mutex_t bool_lock1, bool_lock2; // mutex locks used for synchronization
	const char * getHost(); // function that returns the host 
        const char * getPort(); // function that returns the port 
        const char * getCid(); // function that returns the client ID

    private:
        bool shutdownVal; // value that keeps track of if the program should shutdown or not
        std::string user_id; // value that stores the name of the user
        const char * host; // value that stores the host number
        const char * port; // value that stores the port number
        const char * cid; // value that stores the client's ID
};

// vim: set expandtab sts=4 sw=4 ts=8 ft=cpp: ----------------------------------
