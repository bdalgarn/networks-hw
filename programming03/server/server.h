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

using namespace std;

#ifndef SERVER_H
#define SERVER_H


int dwld(string, int);
int upld(string, int);
int delf(string, int);
int list();
int mdir(string, int);
int rdir(string, int);
int cdir(string, int);
int quit();

#endif
