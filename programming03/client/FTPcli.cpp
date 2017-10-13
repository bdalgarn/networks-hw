#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <time.h>
#include <sys/time.h>
#include "client.h"


#include <arpa/inet.h>
#define MAX_LINE 2048 // max number of bytes we can get at once
using namespace std;


int main(int argc, char *argv[]){
        struct hostent *hp;
        int sock, numbytes;
        char buf[MAX_LINE];
        struct sockaddr_in server;


	if (argc != 3){
	  exit(1);
	}

	char * HOST = argv[1];
	int PORT = atoi(argv[2]);


        /* translate host name into IP address */
        hp = gethostbyname(HOST);
        if (!hp) {
                fprintf(stderr, "Error: unknown host: %s\n", HOST);
                exit(1);
        }

        /* build address data structure */
        bzero((char *)&server, sizeof(server));
        server.sin_family = AF_INET;
        //server.sin_socktype = SOCK_STREAM;
        bcopy(hp->h_addr, (char *)&server.sin_addr, hp->h_length);
        server.sin_port = htons(PORT);

        /* Builds Socket */
        if ((sock = socket(PF_INET,SOCK_STREAM,0)) == -1) {
                 perror("client: socket");
                 return EXIT_FAILURE;
        }

        /* Builds Connection */
        if (connect(sock, (struct sockaddr*)&server,sizeof(server)) == -1) {
                perror("client: connect");
                return EXIT_FAILURE;
        }


	while (1) {
		bzero((char *)&buf, sizeof(buf));
        	fgets(buf,sizeof(buf),stdin);
		// Check message to handle response correctly 
		char operation[5];
		short filename_len;
		char filename[64];


		if (!strncmp(buf, "DWLD", 4)) {
			
			// Store info sent to server
			sscanf(buf, "%s %hi %s", operation, &filename_len, filename);
			dwld(sock,server,filename,filename_len);
		}
		else if (!strncmp(buf, "UPLD", 4)) {
			
			sscanf(buf, "%s %hi %s", operation, &filename_len, filename);
			upld(sock,server,filename,filename_len);
		}
		else if (!strncmp(buf, "DELF", 4)) {
		   
		  sscanf(buf, "%s %hi  %s", operation, &filename_len, filename);
		  delf(sock, server, filename, filename_len);

		}
		else if (!strncmp(buf, "LIST", 4)) {
		  list_func(sock, server);
		}
		else if (!strncmp(buf, "MDIR", 4)) {
 		  // Store info sent to server                                                                  
		  sscanf(buf, "%s %hi  %s", operation, &filename_len, filename);
		  mdir(sock, server, filename, filename_len);
		}
		else if (!strncmp(buf, "RDIR", 4)) {
		  sscanf(buf, "%s %hi  %s", operation, &filename_len, filename);
		  rdir(sock, server, filename, filename_len);	
		}
		else if (!strncmp(buf, "CDIR", 4)) {
		  sscanf(buf, "%s %hi  %s", operation, &filename_len, filename);
		  cdir(sock, server, filename, filename_len);
	
		}
		else if (!strncmp(buf, "QUIT", 4)) {
		  break;
		}
	}
        close(sock);

    return 0;
}
