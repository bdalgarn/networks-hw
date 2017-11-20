// thread.h: PS Thread Library -------------------------------------------------
/*                   
Brianna Hoelting bhoeltin
Anthony DiFalco adifalco 
Ben Dalgarn bdalgarn 

This is the header file for the thread class. 

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

typedef void *(*thread_func)(void *); // creates the function declaration

class Thread {
    public:
        void start(thread_func, void *); // function that starts the thread
        void join(void **); // function that joins the thread
        void detach(); // function that detaches the thread

    private:
        pthread_t p; // instance of a thread

};
