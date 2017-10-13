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

#ifndef CLIENT_H
#define CLIENT_H
 

void dwld(int sock, struct sockaddr_in server, char * filename, int filename_len);
void upld(int sock, struct sockaddr_in server, char * filename, int filename_len);
void mdir(int sock, struct sockaddr_in server, char * filename, short filename_len );
void cdir(int sock,  struct sockaddr_in server, char *filename, int32_t filename_len);
void rdir(int sock,  struct sockaddr_in server, char *filename, int32_t filename_len);
void list_func(int sock, struct sockaddr_in server);
void delf(int sock,  struct sockaddr_in server, char *filename, int32_t filename_len);

#endif
