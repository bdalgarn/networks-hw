#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <sys/time.h>  
#include "server.h"


using namespace std;

#define MAX_LINE 256

int main(int argc, char * argv[]){

  if (argc != 2){
    cout << "here" << endl;
    exit(1);
    
  }
  struct sockaddr_in sin;

  char buf[MAX_LINE];

  int port_number = atoi(argv[1]);
  cout << port_number << endl;
  int len;
  int s, new_s;
  int opt = 1;

  bzero((char *)&sin, sizeof(sin));
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = htonl(INADDR_ANY); //Use the default IP address of server                                            
  sin.sin_port = htons(port_number);

  if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("simplex-talk:       socket");
    exit(1);
  }

  if    ((setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *)& opt, sizeof(int)))<0){
    perror      ("simplex-talk:setscokt");
    exit(1);
  }

  if    ((bind(s,(struct sockaddr*)&sin, sizeof(sin))) < 0) {
    perror("simplex-talk:       bind");
    exit(1);
  }

  if    ((listen(s, 1))<0){
    perror("simplex-talk:       listen");
    exit(1);
  }



  while(1){
    if((new_s=accept(s,(struct sockaddr*)&sin, (socklen_t *)&len))<0){
      perror("simplex-talk:accept");
      exit(1);
    }
    // Receives from client until "QUIT" is called 
    int cont = 1;
    while (cont) {
    	if((len=recv(new_s,buf,sizeof(buf),0))==-1){
      	perror("ServerReceivedError!");
      	exit(1);}
    	if(len==0)break;
	printf("TCPServerReceived:%s", buf);
    	if (!strncmp(buf, "DWLD", 4)) {
    	}
    	else if (!strncmp(buf, "UPLD ", 4)) {
    	}
    	else if (!strncmp(buf, "DELF", 4)) {
    	}
    	else if (!strncmp(buf, "LIST", 4)) {
    	}
    	else if (!strncmp(buf, "MDIR", 4)) {
   	}
    	else if (!strncmp(buf, "RDIR", 4)) {
 	}
    	else if (!strncmp(buf, "CDIR", 4)) {
    	}
    	else if (!strncmp(buf, "QUIT", 4)) {
		// Close sockets
		cont = 0;
    	}
    }
  }
}

void dwld(char *buffer, int buf_len){}
void upld(char *buffer, int buf_len){}
void delf(char *buffer, int buf_len){}
void list(){}
void mdir(char *buffer, int buf_len){}
void rdir(char *buffer, int buf_len){}
void cdir(char *buffer, int buf_len){}
void quit() {
	printf("Quit\n");
        exit(1);
}