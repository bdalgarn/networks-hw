/* client.cpp                                                                                                               
Brianna Hoelting bhoeltin                                                                                                   
Ben Dalgarn bdalgarn                                                                                                        
Anthony DiFalco adifalco                                                                                                    
                                                                                                                            
                                                                                                                            
This is the implementation of all the operations that the client will complete. It is linked to the header files in the client.h.                                                                                                                      
                                                                                                                            
*/

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
#include "client.h"

using namespace std;

#define MAX_LINE 2048

void dwld(int sock, struct sockaddr_in server, char * filename, int filename_len){

  char buf[2048];
  bzero((char *)&buf, sizeof(buf));
  sprintf(buf, "DWLD %hi %s", filename_len, filename);
  if ((sendto(sock,buf,sizeof(buf),0,(struct sockaddr *)&server, sizeof(struct sockaddr))) < 0)
    {
      fprintf(stderr,"[Sendto] : %s",strerror(errno));
      exit(1);
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
    int bytes_remaining = file_size;
    bzero((char *)&buf, sizeof(buf));

    // Set up timer                                                                       
    struct timeval begin_tv;
    struct timeval end_tv;
    gettimeofday(&begin_tv, NULL);
    while (bytes_remaining > 0) {
      int bytes_to_read;
      if (bytes_remaining < 2048) {
	bytes_to_read = bytes_remaining;
      }
      else {
	bytes_to_read = 2048;
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



void upld(int sock, struct sockaddr_in server, char * filename, int filename_len){
  char buf[2048];
  bzero((char *)&buf, sizeof(buf));
  sprintf(buf, "UPLD %hi %s", filename_len, filename);
  if ((sendto(sock,buf,sizeof(buf),0,(struct sockaddr *)&server, sizeof(struct sockaddr))) < 0)
    {
      fprintf(stderr,"[Sendto] : %s",strerror(errno));
      exit(1);
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
      printf("Size of file: %d\n", filesize);
      rewind(fp);
    }
    bzero((char *)&buf, sizeof(buf));
    sprintf(buf, "%d", filesize);
    send(sock, buf, sizeof(buf), 0);

    // Send file to server                                                                

    int bytes_remaining = filesize;
    while (bytes_remaining > 0) {
      int bytes_to_send;
      if (bytes_remaining < 2048) {
	bytes_to_send = bytes_remaining;
      }
      else {
	bytes_to_send = MAX_LINE;
      }
      bytes_remaining -= bytes_to_send;
      bzero((char *)&buf, sizeof(buf));
      fread((void *)&buf, bytes_to_send, 1, fp);
      send(sock, buf, bytes_to_send, 0);
      //printf("bytes_to_send: %d, bytes_remaining: %d\n", bytes_to_send, bytes_remaining);
    }
    fclose(fp);
    char tpMssg[64];
    bzero((char *)&buf, sizeof(buf));
    recv(sock, buf, sizeof(buf), 0);
    memcpy(tpMssg, buf, sizeof(tpMssg));
    printf("File upload successful.\n");
    printf("%s\n", tpMssg);


  }


}


 void mdir(int sock, struct sockaddr_in server, char * filename, short filename_len ){
     int bytesRec;
     char buf[2048];
     bzero((char *)&buf, sizeof(buf));
     sprintf(buf, "MDIR %hi %s", filename_len, filename);
     //printf("%s\n",buf);
     if ((sendto(sock,buf,sizeof(buf),0,(struct sockaddr *)&server, sizeof(struct sockaddr))) < 0){
       fprintf(stderr,"[Sendto] : %s",strerror(errno));
       exit(1);
     }
     bzero(buf, sizeof(buf));
     if ((bytesRec=recv(sock,buf,sizeof(buf),0))<0){
       fprintf(stderr,"[recv] : %s",strerror(errno));
       exit(1);
     }
     if (!strncmp(buf,"-2",2)){
       fprintf(stdout,"The directory already exists\n");
       return;
     }else if (!strncmp(buf,"-1", 2)){
       fprintf(stdout,"Error making the directory\n");
       return;
     }else{
       fprintf(stdout,"Directory made\n");
       return;
     }
     return;
   }

void cdir(int sock,  struct sockaddr_in server, char *filename, int32_t filename_len){
  int bytesRec;
  char buf[2048];
  bzero((char *)&buf, sizeof(buf));
  sprintf(buf, "CDIR %hi %s", filename_len, filename);
  //printf("%s\n",buf);
  if ((sendto(sock,buf,sizeof(buf),0,(struct sockaddr *)&server, sizeof(struct sockaddr))) < 0){
    fprintf(stderr,"[Sendto] : %s",strerror(errno));
    exit(1);
  }
  bzero(buf, sizeof(buf));
  if ((bytesRec=recv(sock,buf,sizeof(buf),0))<0){
    fprintf(stderr,"[recv] : %s",strerror(errno));
    exit(1);
  }
  if (!strncmp(buf,"-2",2)){
    fprintf(stdout,"The directory does not exist\n");
    return;
  }else if (!strncmp(buf,"-1", 2)){
    fprintf(stdout,"Error changing the directory\n");
    return;
  }else{
    fprintf(stdout,"Directory changed\n");
    return;
  }

}

void rdir(int sock,  struct sockaddr_in server, char *filename, int32_t filename_len){
  int bytesRec;
  char buf[2048];
  bzero((char *)&buf, sizeof(buf));
  sprintf(buf, "RDIR %hi %s", filename_len, filename);
  //printf("%s\n",buf);
  if ((sendto(sock,buf,sizeof(buf),0,(struct sockaddr *)&server, sizeof(struct sockaddr))) < 0){
    fprintf(stderr,"[Sendto] : %s",strerror(errno));
    exit(1);
  }
  bzero(buf, sizeof(buf));
  if ((bytesRec=recv(sock,buf,sizeof(buf),0))<0){
    fprintf(stderr,"[recv] : %s",strerror(errno));
    exit(1);
  }
  if (!strncmp(buf,"-1",2)){
    fprintf(stdout,"The directory does not exist on the server\n");
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
    if(!strncmp(buf, "1", 1)) {
    	printf("Directory deleted\n");
    }
    else {
	printf("Failed to delete directory\n");
    }
  }
  else return;
}



void list_func(int sock, struct sockaddr_in server){
  if ((sendto(sock,"LIST",7,0,(struct sockaddr *)&server, sizeof(struct sockaddr))) < 0\
      ){
    fprintf(stderr,"[Sendto] : %s",strerror(errno));
    exit(1);
  }

  char buf[2048];
  bzero((char *)&buf,sizeof(buf));
  if ((recv(sock,buf,2048,0))<0){
    fprintf(stderr,"[LIST recv] : %s",strerror(errno));
  }
  short size;
  sscanf(buf,"%hi",&size);
  if (size <= 0) return;

  bzero((char *)&buf,sizeof(buf));
  if ((recv(sock,buf,2048,0))<0){
    fprintf(stderr,"[LIST recv] : %s",strerror(errno));
  }
  printf("%s\n", buf);
}

void delf(int sock,  struct sockaddr_in server, char *filename, int32_t filename_len){
  //printf("here");
  char buf[2048];
  bzero((char *)&buf, sizeof(buf));
  sprintf(buf, "DELF %hi %s", filename_len, filename);
  //printf("%s\n",buf);
  if ((sendto(sock,buf,sizeof(buf),0,(struct sockaddr *)&server, sizeof(struct sockaddr))) < 0){
    fprintf(stderr,"[Sendto] : %s",strerror(errno));
    exit(1);
  }
  bzero(buf, sizeof(buf));
  if ((recv(sock,buf,sizeof(buf),0))<0){
    fprintf(stderr,"[recv] : %s",strerror(errno));
    exit(1);
  }
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
      if ((recv(sock,buf,sizeof(buf),0))<0){
	fprintf(stderr,"[recv] : %s",strerror(errno));
	exit(1);
      }
      printf("%s\n", buf);
    }
    else return;



  }
}
