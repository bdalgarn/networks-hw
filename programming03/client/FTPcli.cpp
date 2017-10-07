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
#define MAXSIZE 100 // max number of bytes we can get at once
using namespace std;

int main(int argc, char *argv[]){
        struct hostent *hp;
        int sock, numbytes;
        char buf[MAXSIZE];
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


        fgets(buf,sizeof(buf),stdin);

        if ((sendto(sock,buf,sizeof(buf),0,(struct sockaddr *)&server, sizeof(struct sockaddr))) < 0){
                fprintf(stderr,"[Sendto] : %s",strerror(errno));
                return EXIT_FAILURE;
        }

	//      printf("client: connecting to %s\n",(char *)server->ai_addr);
           // This is where our transfers will take place. We will need:
           //   -->  query the server
           //   --> Perform secondary transaction
           //           --> Download
           //           --> Upload
           //           --> Delete
           //           --> Create Dir
           //           --> Delete Dir

        close(sock);

    return 0;
}


