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
#include "server.h"
#include <iostream>


using namespace std;

#define MAX_LINE 2048

void parseOperation(char *, int);

void dwld(int new_s, char *filename, int filename_len){
  char buf[MAX_LINE];

  FILE *fp = fopen(filename, "r");
  int size;
  if (fp == NULL) {
    size = -1;
    printf("File not in local directory\n");
  }
  else {
    fseek(fp, 0L, SEEK_END);
    size = ftell(fp);
    printf("Size of file: %d\n", size);
    rewind(fp);
  }

  // Responds to client                                                                                               
  bzero((char *)&buf, sizeof(buf));
  sprintf(buf, "%d", size);
  send(new_s, buf, sizeof(buf), 0);

  // Sends file to client                                                                                             
  if (size >= 0) {
    int bytes_remaining = size;
    size_t bytes_sent = 0;
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
      bytes_sent += send(new_s, buf, bytes_to_send, 0);
      printf("bytes_to_send: %d, bytes_remaining %d\n", bytes_to_send, bytes_remaining);
    }
    fclose(fp);
  }

}
void upld(int new_s, char * filename, short filename_len){
  char buf[MAX_LINE];

  // Checks for size of file                                                                                          
  FILE *fp = fopen(filename, "r");
  int size;
  if (fp == NULL) {
    size = -1;
    printf("File not in local directory\n");
  }
  else {
    fseek(fp, 0L, SEEK_END);
    size = ftell(fp);
    printf("Size of file: %d\n", size);
    rewind(fp);
  }

  // Responds to client                                                                                               
  bzero((char *)&buf, sizeof(buf));
  sprintf(buf, "%d", size);
  send(new_s, buf, sizeof(buf), 0);

  // Sends file to client                                                                                             
  if (size >= 0) {
    int bytes_remaining = size;
    size_t bytes_sent = 0;
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
      bytes_sent += send(new_s, buf, bytes_to_send, 0);
      printf("bytes_to_send: %d, bytes_remaining %d\n", bytes_to_send, bytes_remaining);
    }
    fclose(fp);
  }




 }




void list_func(int new_s){
  char *cmd = "ls -l";
  char buf[MAX_LINE];
  char cat[MAX_LINE];
  FILE *listing;
  listing = popen(cmd,"r");
  int32_t size = sizeof(listing);
  sprintf(buf,"%d",size);
  if ((send(new_s,buf,sizeof(buf),0))<0){
    fprintf(stderr,"[MDIR sendto #1] : %s",strerror(errno));
    exit(1);
  }
  if (listing){
    while (!feof(listing)){
      if (fgets(buf,MAX_LINE,listing)!=NULL){
        strcat(cat,buf);
      }
    }
    cout <<cat;
    if ((send(new_s,cat,sizeof(buf),0))<0){
      fprintf(stderr,"[MDIR sendto #1] : %s",strerror(errno));
      exit(1);
    }
  }


}


void cdir(int new_s,  struct sockaddr_in sin, char *name, int32_t size){
  int sizeRec;
  char buf[MAX_LINE];
  DIR *d = opendir(name);
  if (d!=NULL){ // Success                                                                                         
    char command[64];
    char return_str[MAX_LINE];
    char temp_com [64];
    strcpy (temp_com, "cd ");
    sprintf(command,"%s%s",temp_com,name);
    strcpy(buf,"1");

    int rtr_val = system(command);
    bzero(buf, MAX_LINE);
    if (rtr_val == 0){ // Successful Sys Call                                                               
      strcpy(buf,"1");
    }else{             // Failed Sys Call                                                                   
      strcpy(buf,"-1");
    }
    closedir(d);
    if ((sendto(new_s,buf,sizeof(buf),0,(struct sockaddr *)&sin,sizeof(struct sockaddr)))<0){
      fprintf(stderr,"[MDIR sendto #1] : %s",strerror(errno));
      exit(1);
    }
  }
 else {
   closedir(d);
   strcpy(buf,"-2");
   if ((sendto(new_s,buf,sizeof(buf),0,(struct sockaddr *)&sin,sizeof(struct sockaddr)))<0){
     fprintf(stderr,"[MDIR sendto #1] : %s",strerror(errno));
     exit(1);
   }
 }
/* Sends Message */ 
}

void rdir(int new_s,  struct sockaddr_in sin, char *name, int32_t size){
  int sizeRec;
  char buf[MAX_LINE];
  DIR *d = opendir(name);
  if (d){ // Success                                                                                         
    char command[64];
    char return_str[MAX_LINE];
    char temp_com [64];
    strcpy (temp_com, "rm ");
    sprintf(command,"%s%s",temp_com,name);
    strcpy(buf,"1");

    if ((sendto(new_s,buf,sizeof(buf),0,(struct sockaddr *)&sin,sizeof(struct sockaddr)))<0){
      fprintf(stderr,"[MDIR sendto #1] : %s",strerror(errno));
      exit(1);
    }

    bzero(buf, MAX_LINE);
    if ((recv(new_s,buf,sizeof(buf),0))<0){
      fprintf(stderr,"[recv] : %s",strerror(errno));
      exit(1);
    }
    if (!strcmp(buf, "YES\n")){
      int rtr_val = system(command);
      bzero(buf, MAX_LINE);
      if (rtr_val == 0){ // Successful Sys Call                                                               
	strcpy(buf,"1");
      }else{             // Failed Sys Call                                                                   
	strcpy(buf,"-1");
      }
      closedir(d);
      if ((sendto(new_s,buf,sizeof(buf),0,(struct sockaddr *)&sin,sizeof(struct sockaddr)))<0){
	fprintf(stderr,"[MDIR sendto #1] : %s",strerror(errno));
	exit(1);
      }
    }
    else return;
  }
  else {
    strcpy(buf,"-2");
    if ((sendto(new_s,buf,sizeof(buf),0,(struct sockaddr *)&sin,sizeof(struct sockaddr)))<0){
      fprintf(stderr,"[MDIR sendto #1] : %s",strerror(errno));
      exit(1);
    }
    return;
  }
  /* Sends Message */ 


}

void mdir(int new_s,  struct sockaddr_in sin, char *name, int32_t size){
  int sizeRec;
  char buf[MAX_LINE];
  DIR *d = opendir(name);
  if (d==NULL){ // Success                                                                                         
    char command[64];
    char return_str[MAX_LINE];
    char temp_com [64];
    strcpy (temp_com, "mkdir ");
    sprintf(command,"%s%s",temp_com,name);
    strcpy(buf,"1");

    int rtr_val = system(command);
    bzero(buf, MAX_LINE);
    if (rtr_val == 0){ // Successful Sys Call                                                               
      strcpy(buf,"1");
    }else{             // Failed Sys Call                                                                   
      strcpy(buf,"-1");
    }
    closedir(d);
    if ((sendto(new_s,buf,sizeof(buf),0,(struct sockaddr *)&sin,sizeof(struct sockaddr)))<0){
      fprintf(stderr,"[MDIR sendto #1] : %s",strerror(errno));
      exit(1);
  }
}
 else {
   closedir(d);
   strcpy(buf,"-2");
   if ((sendto(new_s,buf,sizeof(buf),0,(struct sockaddr *)&sin,sizeof(struct sockaddr)))<0){
     fprintf(stderr,"[MDIR sendto #1] : %s",strerror(errno));
     exit(1);
   }
 }
/* Sends Message */ 
}



void quit() {
  printf("Quit\n");
  exit(1);
}
void delf(int new_s,  struct sockaddr_in sin, char *name, int32_t size){
  int sizeRec;
  char buf[MAX_LINE];

  FILE *fd = fopen(name,"r");
  if (fd){ // Success                                                                                                 
    char command[64];
    char return_str[MAX_LINE];
    char temp_com [64];
    strcpy (temp_com, "rm ");
    sprintf(command,"%s%s",temp_com,name);
    strcpy(buf,"1");

    if ((sendto(new_s,buf,sizeof(buf),0,(struct sockaddr *)&sin,sizeof(struct sockaddr)))<0){
      fprintf(stderr,"[MDIR sendto #1] : %s",strerror(errno));
      exit(1);
    }

    bzero(buf, MAX_LINE);
    if ((recv(new_s,buf,sizeof(buf),0))<0){
      fprintf(stderr,"[recv] : %s",strerror(errno));
      exit(1);
    }
    if (!strcmp(buf, "YES\n")){
      int rtr_val = system(command);
      bzero(buf, MAX_LINE);
      if (rtr_val == 0){ // Successful Sys Call                                                                       
        sprintf(return_str,"%s was succesfully deleted\n",name);
        strcpy(buf,return_str);
      }else{             // Failed Sys Call                                                                           
        sprintf(return_str,"%s was not deleted\n",name);
        strcpy(buf,return_str);
        fclose(fd);
      }

      if ((sendto(new_s,buf,sizeof(buf),0,(struct sockaddr *)&sin,sizeof(struct sockaddr)))<0){
        fprintf(stderr,"[MDIR sendto #1] : %s",strerror(errno));
        exit(1);
      }
    }
    else return;



  }
  else {
    strcpy(buf,"-1");
    if ((sendto(new_s,buf,sizeof(buf),0,(struct sockaddr *)&sin,sizeof(struct sockaddr)))<0){
      fprintf(stderr,"[MDIR sendto #1] : %s",strerror(errno));
      exit(1);
    }
  }
  /* Sends Message */
}

