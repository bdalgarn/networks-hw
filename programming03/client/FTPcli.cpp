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

#include <arpa/inet.h>
#define MAX_LINE 2048 // max number of bytes we can get at once
using namespace std;

void delf(int,  struct sockaddr_in, char *, int32_t);
void list_func(int, struct sockaddr_in);
void mdir(int, struct sockaddr_in);

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

		if (!strncmp(buf, "DWLD", 4)) {
			char operation[5];
			short size;
			char filename[64];
			// Store info sent to server
			sscanf(buf, "%s %hi %s", operation, &size, filename);
			printf("%s %hi %s\n", operation, size, filename);
			// Get response from server
			size = strlen(filename);
			bzero((char *)&buf, sizeof(buf));
			sprintf(buf, "%s %hi %s", operation, size, filename);
			if ((sendto(sock,buf,sizeof(buf),0,(struct sockaddr *)&server, sizeof(struct sockaddr))) < 0){
			  fprintf(stderr,"[Sendto] : %s",strerror(errno));
			  return EXIT_FAILURE;
			}
			bzero(buf, sizeof(buf));
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
				bzero((char *)&buf, sizeof(buf));
				
				// Set up timer
				struct timeval begin_tv;
				struct timeval end_tv;
				gettimeofday(&begin_tv, NULL);

				while (bytes_remaining > 0) {
					int bytes_to_read;
					if (bytes_remaining < MAX_LINE) {
						bytes_to_read = bytes_remaining;
					}
					else {
						bytes_to_read = MAX_LINE;
					}
					bytes_remaining -= bytes_to_read;
					recv(sock, buf, bytes_to_read, 0);   
					fwrite((void *)&buf, bytes_to_read, 1, fp);	
				}
				fclose(fp);
				gettimeofday(&end_tv, NULL);
				int time_microsec = (end_tv.tv_sec * 1000000 + end_tv.tv_usec) - (begin_tv.tv_sec * 1000000 + begin_tv.tv_usec);
				double time_sec = (double)time_microsec / 1000000.0;
				double throughput = (double)file_size / time_sec / 1000000.0; // In MB/s
				printf("The file download was successful.\n");
				printf("%d bytes read in %.2lf seconds: %.2lf Megabytes/sec\n", file_size, time_sec, throughput);
			}
		}
		else if (!strncmp(buf, "UPLD", 4)) {
			char operation[5];
			short size;
			char filename[64];
			// Store info sent to server
			sscanf(buf, "%s %hi %s", operation, &size, filename);
			// Get response from server
			size = strlen(filename);
			bzero((char *)&buf, sizeof(buf));
			sprintf(buf, "%s %hi %s", operation, size, filename);
			if ((sendto(sock,buf,sizeof(buf),0,(struct sockaddr *)&server, sizeof(struct sockaddr))) < 0){
			  fprintf(stderr,"[Sendto] : %s",strerror(errno));
			  return EXIT_FAILURE;
			}
			bzero(buf, sizeof(buf));
			recv(sock, buf, sizeof(buf), 0); 
			int readyToUpload;
			sscanf(buf, "%d", &readyToUpload); 
			if (readyToUpload != 1) {
				printf("Error uploading\n");
			}
			else { 
				// Get file size
				FILE *fp = fopen(filename, "r");
				int filesize;
				if (fp == NULL) {
					filesize = -1;
					printf("File not in local directory\n");
				}
				else {
					fseek(fp, 0L, SEEK_END);
					filesize = ftell(fp);
					rewind(fp);
				}
				bzero((char *)&buf, sizeof(buf));
				sprintf(buf, "%d", filesize);
				send(sock, buf, sizeof(buf), 0);

				// Send file to server
				
				int bytes_remaining = filesize;
				while (bytes_remaining > 0) {
					int bytes_to_send;
					if (bytes_remaining < MAX_LINE) {
						bytes_to_send = bytes_remaining;
					}
					else {
						bytes_to_send = MAX_LINE;
					}
					bytes_remaining -= bytes_to_send;
					bzero((char *)&buf, sizeof(buf));
					fread((void *)&buf, bytes_to_send, 1, fp);
					send(sock, buf, bytes_to_send, 0);
				}
				fclose(fp);

				// Get throughput info from server
				char tpMssg[64];
				bzero((char *)&buf, sizeof(buf));
				recv(sock, buf, sizeof(buf), 0);
				memcpy(tpMssg, buf, sizeof(tpMssg));
				printf("File upload successful.\n");
				printf("%s\n", tpMssg);
			}
		}
		else if (!strncmp(buf, "DELF", 4)) {
		  char operation[5];
		  short filename_len;
		  char filename[64];
		  // Store info sent to server                                                                  
		  sscanf(buf, "%s %s", operation, filename);
		  filename_len = strlen(filename);
		  delf(sock, server, filename, filename_len);

		}
		else if (!strncmp(buf, "LIST", 4)) {
		  list_func(sock, server);
		}
		else if (!strncmp(buf, "MDIR", 4)) {
		  mdir(sock,server);
		}
		else if (!strncmp(buf, "RDIR", 4)) {

		}
		else if (!strncmp(buf, "CDIR", 4)) {

		}
		else if (!strncmp(buf, "QUIT", 4)) {
		  break;
		}
	}
        close(sock);

    return 0;
}

void mdir(int sock, struct sockaddr_in server ){
  char buf[MAX_LINE];
  bzero((char *)&buf,sizeof(buf));
  fgets(buf,sizeof(buf),stdin);
  short name_len;
  char first[MAX_LINE] = "";
  sprintf(first,"%hi %s",&name_len,buf);
  if ((sendto(sock,first,sizeof(first),0,(struct sockaddr *)&server, sizeof(struct sockaddr))) \
      < 0){
    fprintf(stderr,"[Sendto] : %s",strerror(errno));
    exit(1);;
  }

  printf("%s\n",first);
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


void list_func(int sock, struct sockaddr_in server){
  if ((sendto(sock,"LIST",7,0,(struct sockaddr *)&server, sizeof(struct sockaddr))) < 0\
      ){
    fprintf(stderr,"[Sendto] : %s",strerror(errno));
    exit(1);
  }

  char buf[MAX_LINE];
  int n;
  bzero((char *)&buf,sizeof(buf));
  if ((n=recv(sock,buf,MAX_LINE,0))<0){
    fprintf(stderr,"[LIST recv] : %S",strerror(errno));
  }
  int32_t size;
  sscanf(buf,"%hi",&size);
  if (size <= 0) return;
  ssize_t counted=0;
  //                      while (counted!=size){                                                                        
  //                              cout <<"beginning of loop\n";                                                         
  bzero((char *)&buf,sizeof(buf));
  if ((n=recv(sock,buf,MAX_LINE,0))<0){
    fprintf(stderr,"[LIST recv] : %S",strerror(errno));
  }
  printf("%s\n", buf);
  //                              counted=+n;      

}


void delf(int sock,  struct sockaddr_in server, char *filename, int32_t filename_len){
  int bytesRec;
  char buf[MAX_LINE];
  bzero((char *)&buf, sizeof(buf));
  sprintf(buf, "DELF %hi %s", filename_len, filename);
  printf("%s\n",buf);
  if ((sendto(sock,buf,sizeof(buf),0,(struct sockaddr *)&server, sizeof(struct sockaddr))) < 0){
    fprintf(stderr,"[Sendto] : %s",strerror(errno));
    exit(1);
  }
  bzero(buf, sizeof(buf));
  if ((bytesRec=recv(sock,buf,sizeof(buf),0))<0){
    fprintf(stderr,"[recv] : %s",strerror(errno));
    exit(1);
  }
  int num;
  if (!strncmp(buf,"-1",2)){
    fprintf(stdout,"The file does not exist on the server\n");
    return;
  }else if (!strncmp(buf,"1", 1)){
    fprintf(stdout,"Are you sure you want to delete (YES/NO) %s?\n", filename);
    bzero(buf, sizeof(buf));
    fgets(buf,sizeof(buf),stdin);
    if ((sendto(sock,buf,sizeof(buf),0,(struct sockaddr *)&server, sizeof(struct sockaddr))) < 0){
      fprintf(stderr,"[Sendto] : %s",strerror(errno));
      exit(1);
    
    }
    if (!strncmp(buf, "NO", 2)){
      fprintf(stdout,"Delete abandoned by user!\n");
      return;
    }
    bzero(buf, sizeof(buf));
    if ((bytesRec=recv(sock,buf,sizeof(buf),0))<0){
      fprintf(stderr,"[recv] : %s",strerror(errno));
      exit(1);
    }
    printf("%s\n", buf);
  }
  else return;
  


}
