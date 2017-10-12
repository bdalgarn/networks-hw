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

void parseOperation(char *, int);

void dwld(char *, int);
void upld(char *, int);
void delf(int, struct sockaddr_in, char *, int);
void list_func(int);
void mdir(int);
void rdir(char *, int);
void cdir(char *, int);
void quit();

#endif
