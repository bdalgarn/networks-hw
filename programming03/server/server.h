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
#include <dirent.h>

using namespace std;

#ifndef SERVER_H
#define SERVER_H

#define MAX_LINE 2048

void parseOperation(char *, int);

void dwld(int, char *, int);
void upld(int, char *, short);
void list_func(int);
void cdir(int,  struct sockaddr_in, char *, int32_t);
void rdir(int,  struct sockaddr_in,  char *, int32_t);
void mdir(int,  struct sockaddr_in, char *, int32_t);
void quit();
void delf(int,  struct sockaddr_in, char *, int32_t);

#endif
