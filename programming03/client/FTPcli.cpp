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

void mdir(char *name, int32_t size){
//    char buf[MAXSIZE]; 
//    if ((size!=recv(new_s,buf,sizeof(buf),0))<0){
//	fprintf(stderr,"[recv] : %s",strerror(errno));
//	return -1;
//    }
//
    char *size = atoi(size);
   
    char *init_msg = "";
    sprintf(init_msg,"%s %s",size,name);

    if (connect(sock, (struct sockaddr*)&server,sizeof(server)) == -1) {
         perror("client: connect");
         return EXIT_FAILURE;
    }

    bzero((char *)&buf, sizeof(buf));
    strcpy(buf,init_msg,sizeof(init_msg));
    if ((sendto(sock,buf,sizeof(buf),0,(struct sockaddr *)&server, sizeof(struct sockaddr))) < 0){
            fprintf(stderr,"[Sendto] : %s",strerror(errno));
            return EXIT_FAILURE;
    }
    if ((size=recv(sock,buf,sizeof(buf),0))<0){
	fprintf(stderr,"[recv] : %s",strerror(errno));
	return -1;
    }
    int num;
    if (!strncmp(buf,"-2",2)){
       fprintf(stdout,"The directory already exists on the server\n");
       continue;
    }else if (!strncmp(buf,"-1",2)){
	fprintf(stdout,"Error making the directory\n");
	continue;
    }else{
	fprintf(stdout,"The directory was successfully made\n");
	continue;
    }
}

void rdir(char *name, int32_t size){
    char *size = atoi(size);
   
    char *init_msg = "";
    sprintf(init_msg,"%s %s",size,name);

    if (connect(sock, (struct sockaddr*)&server,sizeof(server)) == -1) {
         perror("client: connect");
         return EXIT_FAILURE;
    }

    bzero((char *)&buf, sizeof(buf));
    strcpy(buf,init_msg,sizeof(init_msg));
    if ((sendto(sock,buf,sizeof(buf),0,(struct sockaddr *)&server, sizeof(struct sockaddr))) < 0){
            fprintf(stderr,"[Sendto] : %s",strerror(errno));
            return EXIT_FAILURE;
    }
    if ((size=recv(sock,buf,sizeof(buf),0))<0){
	fprintf(stderr,"[recv] : %s",strerror(errno));
	return -1;
    }
    int num;
    if (!strncmp(buf,"-2",2) || !strncmp(buf,"-1",2)){
       fprintf(stdout,"Failed to delete directory\n");
       continue;
    }else{
	fprintf(stdout,"The directory was successfully deleted\n");
	continue;
    }
}

void cdir(char *name, int32_t size){

    char *size = atoi(size);
   
    char *init_msg = "";
    sprintf(init_msg,"%s %s",size,name);

    if (connect(sock, (struct sockaddr*)&server,sizeof(server)) == -1) {
         perror("client: connect");
         return EXIT_FAILURE;
    }

    bzero((char *)&buf, sizeof(buf));
    strcpy(buf,init_msg,sizeof(init_msg));
    if ((sendto(sock,buf,sizeof(buf),0,(struct sockaddr *)&server, sizeof(struct sockaddr))) < 0){
            fprintf(stderr,"[Sendto] : %s",strerror(errno));
            return EXIT_FAILURE;
    }
    if ((size!=recv(sock,buf,sizeof(buf),0))<0){
	fprintf(stderr,"[recv] : %s",strerror(errno));
	return -1;
    }

    if (!strncmp(buf,"-2",2)){
       fprintf(stdout,"The directory does exist on the server\n");
       continue;
    }else if (!strncmp(buf,"-1",2)){
	fprintf(stdout,"Error changing into the directory\n");
	continue;
    }else{
	fprintf(stdout,"Successfully changed directory\n");
	continue;
    }

}

