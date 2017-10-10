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


using namespace std;

#define MAX_LINE 256

int main(int argc, char * argv[]){

  if (argc != 2){
    cout << "here" << endl;
    exit(1);
    
  }
  struct sockaddr_in sin;

  char buf[MAX_LINE];

  int port_number = atoi(argv[1]);
  cout << port_number << endl;
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
    	if((len=recv(new_s,buf,sizeof(buf),0))==-1){
      	perror("ServerReceivedError!");
      	exit(1);}
    	if(len==0)break;
    	if (!strncmp(buf, "DWLD", 4)) {
		char operation[5];
		short int filename_len;
		char filename[64];
		// Decodes information sent by client
		sscanf(buf, "%s %hi %s", operation, &filename_len, filename);
		printf("%s %hi %s\n", operation, filename_len, filename);

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
		}

		// Responds to client
		bzero((char *)&buf, sizeof(buf));
		sprintf(buf, "%d", size);
		send(new_s, buf, sizeof(buf), 0); 
    	}
    	else if (!strncmp(buf, "UPLD ", 4)) {
    	}
    	else if (!strncmp(buf, "DELF", 4)) {
	  sscanf(buf, "%s %hi %s", operation, &filename_len, filename);
	  delf(s, filename, filename_len);
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
		// Close sockets
		cont = 0;
    	}
    }
  }
}

void dwld(char *buffer, int buf_len){}
void upld(char *buffer, int buf_len){}
void list(){}
void mdir(char *buffer, int buf_len){
/*    char buf[MAXSIZE];
    if ((size!=recv(new_s,buf,sizeof(buf),0))<0){
	fprintf(stderr,"[recv] : %s",strerror(errno));
	return -1;
    }
    char *root_path = "../root_dir"
    char *full_path = "";
    sprintf(full_path,"%s%s",root_path,name);
    DIR *d = opendir(full_path);
    if (d == NULL){ // Success
	 char *command;
	 char *temp_com = "mkdir ";
         sprintf(command,"%s%s",temp_com,full_path);
	 int rtr_val = system(command);
	 if (rtr_val == 0){ // Successful Sys Call
	    strcpy(buf,"1");	
	 }else{             // Failed Sys Call
	    strcpy(buf,"-1");
	 }
    }else{
	strcpy(buf,"-2"); // Failure
    } 
*/    /* Sends Message */
/*    if ((sendto(new_s,buf,sizeof(buf),0,(struct sockaddr *)&new_s,sizepf(struct sockaddr)))<0){
	fprintf(stderr,"[MDIR sendto #1] : %s",strerror(errno));
	continue;
    }
*/
}
void rdir(char *buffer, int buf_len){
/*    char buf[MAXSIZE];
    if ((size!=recv(new_s,buf,sizeof(buf),0))<0){
	fprintf(stderr,"[recv] : %s",strerror(errno));
	return -1;
    }
    char *root_path = "../root_dir"
    char *full_path = "";
    sprintf(full_path,"%s%s",root_path,name);
    DIR *d = opendir(full_path);
    if (d != NULL){ // Success
	 char *command;
	 char *temp_com = "rm ";
         sprintf(command,"%s%s",temp_com,full_path);
	 int rtr_val = system(command);
	 if (rtr_val == 0){ // Successful Sys Call
	    strcpy(buf,"1");	
	 }else{             // Failed Sys Call
	    strcpy(buf,"-1");
	 }
    }else{
	strcpy(buf,"-2"); // Failure
    } 
*/    /* Sends Message */
/*    if ((sendto(new_s,buf,sizeof(buf),0,(struct sockaddr *)&new_s,sizepf(struct sockaddr)))<0){
	fprintf(stderr,"[MDIR sendto #1] : %s",strerror(errno));
	continue;
    }
*/

}


void cdir(char *buffer, int buf_len){
/*    char buf[MAXSIZE];
    if ((size!=recv(new_s,buf,sizeof(buf),0))<0){
	fprintf(stderr,"[recv] : %s",strerror(errno));
	return -1;
    }
    char *root_path = "../root_dir"
    char *full_path = "";
    sprintf(full_path,"%s%s",root_path,name);
    DIR *d = opendir(full_path);
    if (d != NULL){ // Success
	 char *command;
	 char *temp_com = "cd ";
         sprintf(command,"%s%s",temp_com,full_path);
	 int rtr_val = system(command);
	 if (rtr_val == 0){ // Successful Sys Call
	    buf = "1";	
	 }else{             // Failed Sys Call
	    buf = "-1";
	 }
    }else{
	buf = "-2"; // Failure
    } 
*/    /* Sends Message */
/*    if ((sendto(new_s,buf,sizeof(buf),0,(struct sockaddr *)&new_s,sizepf(struct sockaddr)))<0){
	fprintf(stderr,"[MDIR sendto #1] : %s",strerror(errno));
	continue;
    }
*/
}
void quit() {
	printf("Quit\n");
        exit(1);
}
void delf(int server_addr, char *name, int32_t size){
  char buf[MAXSIZE];
  if ((size=recv(server_addr,buf,sizeof(buf),0))<0){
    fprintf(stderr,"[recv] : %s",strerror(errno));
    return -1;
  }
    char *root_path = "../"
      char *full_path = "";
    sprintf(full_path,"%s%s",root_path,name);
    FILE *fd = fopen(full_path,"r");
    if (fd){ // Success                                                                                               
      char *command,return_str;
      char *temp_com = "rm ";
      sprintf(command,"%s%s",temp_com,full_path);
      strcpy(buf,"1");
      if ((sendto(server_addr,buf,sizeof(buf),0,(struct sockaddr *)&new_s,sizepf(struct sockaddr)))<0){
	fprintf(stderr,"[MDIR sendto #1] : %s",strerror(errno));
	return -1;
      }
      bzero(buf, MAXSIZE);
      if ((recv(server_addr,buf,sizeof(buf),0))<0){
        fprintf(stderr,"[recv] : %s",strerror(errno) < 0);
        return -1;
      }
      if (strcmp(buf, "YES") == 0){\
        int rtr_val = system(command);
        bzero(buf, MAXSIZE);
        if (rtr_val == 0){ // Successful Sys Call                                                                    
          sprintf(return_str,"%s was succesfully deleted\n",name);
          strcpy(buf,return_str);
        }else{             // Failed Sys Call                                                                        
          sprintf(return_str,"%s was not deleted\n",name);
          strcpy(buf,return_str);
        }
      }else{
        return;
      }

    }else{            // Failed Sys Call                                                                              
      strcpy(buf,"-1");
    }
    /* Sends Message */
    if ((sendto(server_addr,buf,sizeof(buf),0,(struct sockaddr *)&new_s,sizepf(struct sockaddr)))<0){
      fprintf(stderr,"[MDIR sendto #1] : %s",strerror(errno));
      continue;
    }
}
