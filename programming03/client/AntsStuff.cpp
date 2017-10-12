#include <stdio.h>
#include <iostream>
#include <iomanip>
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
#define MAX_LINE 2048 // max number of bytes we can get at once
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
			char operation[5];
			short size;
			char filename[64];
			// Store info sent to server
			sscanf(buf, "%s %hi %s", operation, &size, filename);
			// Get response from server
			bzero((char *)&buf, sizeof(buf));
			recv(sock, buf, sizeof(buf), 0); 
			int file_size;
			sscanf(buf, "%d", &file_size); 
			if (file_size < 0) {
				printf("That file does not exist\n");
			}
			else {
				FILE *fp = fopen(filename, "w");
				printf("File size: %d\n", file_size);
				int bytes_remaining = file_size;
				size_t bytes_read = 0;
				bzero((char *)&buf, sizeof(buf));
				while (bytes_remaining > 0) {
					int bytes_to_read;
					if (bytes_remaining < MAX_LINE) {
						bytes_to_read = bytes_remaining;
					}
					else {
						bytes_to_read = MAX_LINE;
					}
					bytes_remaining -= bytes_to_read;
					printf("bytes to read: %d, bytes remaining: %d\n", bytes_to_read, bytes_remaining);
					bytes_read += recv(sock, buf, bytes_to_read, 0);   
					fwrite((void *)&buf, bytes_to_read, 1, fp);	
				}
				fclose(fp);
				printf("The file download was successful.\n");
			}
		}
		else if (!strncmp(buf, "UPLD", 4)) {
			
		}
		else if (!strncmp(buf, "DELF", 4)) {

		}
		else if (!strncmp(buf, "LIST", 4)) {
			int n;
			bzero((char *)&buf,sizeof(buf));
			if ((n=recv(sock,buf,MAX_LINE,0))<0){
				fprintf(stderr,"[LIST recv] : %S",strerror(errno));
			}
			int32_t size;
			sscanf(buf,"%hi",&size);
			if (size <= 0)continue;
			ssize_t counted=0;
//			while (counted!=size){
//				cout <<"beginning of loop\n";
				bzero((char *)&buf,sizeof(buf));
				if ((n=recv(sock,buf,MAX_LINE,0))<0){
					fprintf(stderr,"[LIST recv] : %S",strerror(errno));
				}
				cout << buf;
//				counted=+n;
//			}
		}
		else if (!strncmp(buf, "MDIR", 4)) {
			bzero((char *)&buf,sizeof(buf));
			fgets(buf,sizeof(buf),stdin);
			short int file_size = sizeof(buf);
			char first[MAX_LINE] = "";
			sprintf(first,"%hi %s",&file_size,buf);
	        	if ((sendto(sock,first,sizeof(first),0,(struct sockaddr *)&server, sizeof(struct sockaddr))) < 0){
        	        	fprintf(stderr,"[Sendto] : %s",strerror(errno));
                		return EXIT_FAILURE;
        		}
			cout << first;
			bzero((char *)&buf,sizeof(buf));
			int n;
			if ((n=recv(sock,buf,MAX_LINE,0))<0){
				fprintf(stderr,"[LIST recv] : %S",strerror(errno));
			}

		        if (!strncmp(buf,"-2",2)){
			       fprintf(stdout,"The directory already exists on the server\n");
		        }else if (!strncmp(buf,"-1",2)){
				fprintf(stdout,"Error making the directory\n");
	        	}else{
				fprintf(stdout,"The directory was successfully made\n");
		        }

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


