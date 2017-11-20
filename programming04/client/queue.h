// queue.h: PS Queue Library -------------------------------------------------
/*  
Brianna Hoelting bhoeltin
Anthony DiFalco adifalco 
Ben Dalgarn bdalgarn 

This is the implementaiton of a thread safe queue. The only thing different from a normal queeue is that it has locks in its push and pop functions. 

*/



#pragma once

#include <queue>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>

#ifndef QUEUE_H
#define QUEUE_H


// macros that perform thread operations and check that they complete with out error
#define PTHREAD_CHECK(f) \
  if ((rc = (f)) != 0) { \
  fprintf(stderr, "%s\n", strerror(rc)); \
  exit(EXIT_FAILURE); \
  }

#define Pthread_mutex_init(l, a) \
  PTHREAD_CHECK(pthread_mutex_init(l, a));

#define Pthread_mutex_lock(l) \
  PTHREAD_CHECK(pthread_mutex_lock((pthread_mutex_t *)(l)));

#define Pthread_mutex_unlock(l) \
  PTHREAD_CHECK(pthread_mutex_unlock((pthread_mutex_t *)(l)));

#define Pthread_cond_init(c, a) \
  PTHREAD_CHECK(pthread_cond_init((c), (a)));

#define Pthread_cond_wait(c, l) \
  PTHREAD_CHECK(pthread_cond_wait((c), (l)));

#define Pthread_cond_signal(c) \
  PTHREAD_CHECK(pthread_cond_signal((pthread_cond_t *)(c)));

const size_t QUEUE_MAX = 1000;

template <typename T>
class Queue { // templated queue class
    private:
        std::queue<T>   data; // uses standard queue class to create a queue so we can use the already implemented functions
        // instances of locks and condition variables
        pthread_mutex_t lock;
        pthread_cond_t  fill;
        pthread_cond_t  empty;

    public:
        Queue() { // queue class constructor
            int rc;
            // initializes the lock and condition variables
            Pthread_mutex_init(&lock, NULL);
            Pthread_cond_init(&fill, NULL);
            Pthread_cond_init(&empty, NULL);
        }

    void push(const T &value) { // queue push function
        int rc;

        Pthread_mutex_lock(&lock); // locks
        data.push(value); // calls the standard queue push to add the value to the queue
        Pthread_cond_signal(&fill); // signals the fill condition variable
        Pthread_mutex_unlock(&lock); // unlocks
    }

    T pop() { // queue pop function
        int rc;

        Pthread_mutex_lock(&lock); // locks
        while (data.empty()) { // as long as the queue isn't empty
            Pthread_cond_wait(&fill, &lock); // wait on the fill condition variable
        }

        T value = data.front(); // gets the first element in the data queue
        data.pop(); // pops the element from the queue
        Pthread_cond_signal(&fill); // signals the fill condition variable
        Pthread_mutex_unlock(&lock); // unlocks
        return value;
    }

    size_t getSize(){
      return data.size();
    }
};

#endif
