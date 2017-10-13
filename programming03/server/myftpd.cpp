#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <sys/time.h>  
#include "server.h"
#include <dirent.h>

using namespace std;


#define MAX_LINE 2048

int main(int argc, char * argv[]){

  if (argc != 2){
    exit(1);
    
  }
  struct sockaddr_in sin;

  char buf[MAX_LINE];

  int port_number = atoi(argv[1]);
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
      char operation[5];
      short filename_len;
      char filename[64];
      bzero(buf, sizeof(buf));
    	len=recv(new_s,buf,sizeof(buf),0);
    	if (!strncmp(buf, "DWLD", 4)) {
		sscanf(buf, "%s %hi %s", operation, &filename_len, filename);
		dwld(new_s, filename, filename_len);
	 }
    	else if (!strncmp(buf, "UPLD ", 4)) {
		sscanf(buf, "%s %hi %s", operation, &filename_len, filename);
		upld(new_s, filename, filename_len);

    	}
    	else if (!strncmp(buf, "DELF", 4)) {

	  sscanf(buf, "%s %hi %s", operation, &filename_len, filename);
	  delf(new_s, sin, filename, filename_len);
	}
    	else if (!strncmp(buf, "LIST", 4)) {
	  list_func(new_s);

    	}
    	else if (!strncmp(buf, "MDIR", 4)) {

	  sscanf(buf, "%s %hi %s", operation, &filename_len, filename);
	  mdir(new_s, sin, filename, filename_len);
	}
    	else if (!strncmp(buf, "RDIR", 4)) {
	  sscanf(buf, "%s %hi %s", operation, &filename_len, filename);
	  rdir(new_s, sin, filename, filename_len);
 	}
    	else if (!strncmp(buf, "CDIR", 4)) {
	  sscanf(buf, "%s %hi %s", operation, &filename_len, filename);
	  cdir(new_s, sin, filename, filename_len);
	
    	}
    	else if (!strncmp(buf, "QUIT", 4)) {
		// Close sockets
		cont = 0;
    	}
    }
  }
}
