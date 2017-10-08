#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#define PORT 41001 // the port c
#define HOST "student00.cse.nd.edu"
#define MAX_LINE 256 // max number of bytes we can get at once
using namespace std;

int main(int argc, char *argv[]){
        struct hostent *hp;
        int sock, numbytes;
        char buf[MAX_LINE];
        struct sockaddr_in server;

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

        	if ((sendto(sock,buf,sizeof(buf),0,(struct sockaddr *)&server, sizeof(struct sockaddr))) < 0){
                	fprintf(stderr,"[Sendto] : %s",strerror(errno));
                	return EXIT_FAILURE;
        	}
		// Check message to handle response correctly 
		if (!strncmp(buf, "DWLD", 4)) {

		}
		else if (!strncmp(buf, "UPLD", 4)) {
			
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

		}
	}
        close(sock);

    return 0;
}


