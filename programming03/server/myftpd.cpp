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
    	len=recv(new_s,buf,sizeof(buf),0);
    	if (!strncmp(buf, "DWLD", 4)) {
		char operation[5];
		short filename_len;
		char filename[64];
		// Decodes information sent by client
		sscanf(buf, "%s %hi %s", operation, &filename_len, filename);

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
			}
			fclose(fp);
		}
    	}
    	else if (!strncmp(buf, "UPLD ", 4)) {
		char operation[5];
		short filename_len;
		char filename[64];
		// Decodes information sent by client
		sscanf(buf, "%s %hi %s", operation, &filename_len, filename);

		// Sends acknowdlegement to client (Sends a 32-bit int == 1)
		bzero((char *)&buf, sizeof(buf));
		sprintf(buf, "%d", 1);
		send(new_s, buf, sizeof(buf), 0);	

		// Receives size of file from client
		bzero((char *)&buf, sizeof(buf));
		recv(new_s, buf, sizeof(buf), 0);
		int file_size;
		sscanf(buf, "%d", &file_size);
		
		// Receive file from client
		FILE *fp = fopen(filename, "w");
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
			recv(new_s, buf, bytes_to_read, 0);
			fwrite((void *)&buf, bytes_to_read, 1, fp);
		}
		fclose(fp);
		gettimeofday(&end_tv, NULL);
		int time_microsec = (end_tv.tv_sec * 1000000 + end_tv.tv_usec) - (begin_tv.tv_sec * 1000000 + begin_tv.tv_usec);
		double time_sec = (double)time_microsec / 1000000.0;
		double throughput = (double)file_size / time_sec / 1000000.0;

		// Send throughput results to client
		bzero((char *)&buf, sizeof(buf));
		sprintf(buf, "%d bytes read in %.2lf seconds: %.2lf Megabytes/sec\n", file_size, time_sec, throughput);
		send(new_s, buf, sizeof(buf), 0);
    	}
    	else if (!strncmp(buf, "DELF", 4)) {
	  char operation[5];
	  short filename_len;
	  char filename[64];

	  sscanf(buf, "%s %hi %s", operation, &filename_len, filename);
	  delf(new_s, sin, filename, filename_len);
	}
    	else if (!strncmp(buf, "LIST", 4)) {
	  list_func(new_s);

    	}
    	else if (!strncmp(buf, "MDIR", 4)) {
	  char operation[5];
	  short filename_len;
	  char filename[64];

	  sscanf(buf, "%s %hi %s", operation, &filename_len, filename);
	  mdir(new_s, sin, filename, filename_len);
	}
    	else if (!strncmp(buf, "RDIR", 4)) {
	  char operation[5];
	  short filename_len;
	  char filename[64];

	  sscanf(buf, "%s %hi %s", operation, &filename_len, filename);
	  rdir(new_s, sin, filename, filename_len);
 	}
    	else if (!strncmp(buf, "CDIR", 4)) {
	  char operation[5];
	  short filename_len;
	  char filename[64];

	  sscanf(buf, "%s %hi %s", operation, &filename_len, filename);
	  ddir(new_s, sin, filename, filename_len);
	
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
    else return;
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
    else return;
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
