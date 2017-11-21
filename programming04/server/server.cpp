/*                   
Brianna Hoelting bhoeltin
Anthony DiFalco adifalco 
Ben Dalgarn bdalgarn 

This is the implementation of the server. It will interact with the client and perform different task based on what the client asks of it. It will give each client their messages as well as a list of online users if applicapble. 

*/


#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <map>
#include <queue>
#include "login.cpp"
#define MAX_SIZE 10
#define DEBUG 0
using namespace std;



void *connection_handler(void *);
void read_it(queue <char *> *, int);	
void write_it(char, char *, map<char *,queue<char *> *> *, int);
void get_message(int sock, int mode, char * message);
void check_error(ssize_t size);

std::map<char *,std::queue<char *> *> queues;
struct arg_t {
		//queue <char *> *user_qs[10];
		void * user_qs;
		char * user;	
		map <char *, queue <char *> *> *user_map; 		   
		int sock;
};

//queue <queue<char *>> qs[10];
//bzero(qs,sizeof(queue)*sizeof(char *));

std::queue <char *> qs[10];
int in_use[10] = {0};

int main(int argc , char *argv[]){

    /* Initializations */

    //bzero(qs,sizeof(queue<char *>)*sizeof(char *)); 
    int sock , client_sock , c , *new_sock;
    struct sockaddr_in server , client;
    bzero((char *)&server,sizeof(server));
    bzero((char *)&client,sizeof(client));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(atoi(argv[1]));
    /* Building Socket */ 
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1){
        printf("Could not create socket");
    }
    /* Bind to group's socket */
    if(bind(sock,(struct sockaddr *)&server , sizeof(server)) < 0){        
	perror("bind failed. Error");
        return 1;         
    }
    /* Listen on Socket */
    listen(sock , 3);
    c = sizeof(struct sockaddr_in);
    /* Accept on Socket */
    while((client_sock=accept(sock,(struct sockaddr *)&client,(socklen_t*)&c))){
        pthread_t client_thread;
        new_sock = (int *)malloc(1);    // Allocate space
        *new_sock = client_sock; // Duplicae address

       char * user_name;// = "Ben";

       map <char *, queue <char *> *> users; 		   
       arg_t args {
		(void *)&qs, // user_qs
		user_name, // user	
		&users, // user_map	   
		*new_sock // sock
       };
        /* Creates New Thread */
        if(pthread_create(&client_thread,NULL,connection_handler,(void*)&args)<0){
            perror("could not create thread");
            return 1;
        }
     }
    /* Check Error */
    if (client_sock < 0){
        perror("accept failed");
        return 1;
    }
    close(sock);
    return 0;
} 


void *connection_handler(void *_args){
    arg_t *args = (arg_t *)_args;
    /* Initializations */
    int sock = args->sock;
    char * to_be_written,user;  // This is going to be what we need to write to the users queue
    char * to_be_recieved; // This is what we need to send to our client (messages from others in their queue)
    ssize_t read_size = BUFSIZ;
    char *message;
    char client_message[BUFSIZ];
    char cat_buf[BUFSIZ];

    // Get username
    recv(sock, (void *)client_message, BUFSIZ, 0);
    char username[BUFSIZ]; 
    strcpy(username, client_message);
    printf("Received username: %s\n", username);
    char password[BUFSIZ]; 
    if (loginUser(username, password) == 1) {
	strcpy(client_message, "Please enter your password: ");
	send(sock, (void *)client_message, BUFSIZ, 0);
	// Get response
    	bzero(&client_message, BUFSIZ);	
    	recv(sock, (void *)client_message, BUFSIZ, 0);
	while (strncmp(client_message, password, strlen(password)) != 0) {
    		bzero(&client_message, BUFSIZ);	
		strcpy(client_message, "Invalid password.");
		send(sock, (void *)client_message, BUFSIZ, 0);
    		bzero(client_message, BUFSIZ);	
    		recv(sock, (void *)client_message, BUFSIZ, 0);
	}
	// Successful login
	strcpy(client_message, "ACK_LOG");
	send(sock, (void *)client_message, BUFSIZ, 0);
    }
    else {
	bzero(&client_message, BUFSIZ);	
	strcpy(client_message, "Please enter a password: ");
	send(sock, (void *)client_message, BUFSIZ, 0);
	// Get response
    	bzero(&password, BUFSIZ);	
    	recv(sock, (void *)password, BUFSIZ, 0);
	handleNewUser(username, password);
	bzero(client_message, BUFSIZ);
	strcpy(client_message, "ACK_REG"); // Acknowledge registration of new user
	send(sock, (void *)client_message, BUFSIZ, 0);
    }


    bool mode = false; // This value is going to determine whether to write to the queues of read from them

    /* Casts vals back from void */
    /*queue <char *> *copy_q[10]; 
    for (int i = 0; i < 10; i++) {
	copy_q[i] = &((queue <char *> *)args->user_qs)[i];
    }*/
    //char * user_name = args->user;
    // Find unused queue
    int q_num = -1;
    for (int i = 0; i < 10; i++) {
	if (!in_use[i]) {
		q_num = i;
		break;
	}
    }
    map<char *,queue<char *> *> *map_ptr = (map<char *,queue<char *> *> *)args->user_map;
    if (q_num >= 0) {
	in_use[q_num] = 1;
	(*map_ptr)[username] = &qs[q_num];
    }
    else {
	void * return_ptr = NULL;
	return return_ptr;
    }	



    while (recv(sock, (void *)client_message, BUFSIZ, 0)) {
      printf("client message: %s\n", client_message);
      if (strncmp(client_message, "E", 1) == 0) {
		close(sock); // Close socket fd
		for (int i = 0; i < 10; i++) {
			if (&qs[i] == (*map_ptr)[username]) {
				in_use[i] = 0;
			}
		}
		map_ptr->erase(username);
		void * ptr = NULL;
		return ptr;
	}
	else if (strncmp(client_message, "B", 1) == 0) {
	  printf("In B if statement\n");
	  write_it('B',username,map_ptr,args->sock);
	  break;
	}

	else if (strncmp(client_message, "P", 1) == 0) {
          write_it('P',username,map_ptr,args->sock);
	}
	else printf("none\n");
    }
	/*
    if(mode){
	    for (int i = 0; i < MAX_SIZE; i++){
		 if (copy_q[i]==user_q){
		   read_it(user_q,args->sock);
		}
	    }
    }*/
    void * return_ptr = NULL;
    return return_ptr;
}



void get_message(int sock, int mode, char * message){
    /* Build Probe */
    char buf[BUFSIZ];
    bzero(buf,BUFSIZ);
    if (mode==1){
       strcpy(buf, "P,Server,Please Enter Your Message : ");
    }else if (mode==0){
       strcpy(buf, "P,Server,Please Enter Recipient : ");
    }else{
    }
    printf("%s\n", buf);
    /* Send Probe */
    ssize_t size = send(sock,(void *)buf,sizeof(buf),0);
    printf("%s\n", buf);
    char * cat = (char *)malloc(BUFSIZ*4);
    bzero(cat,sizeof(cat));
    if (DEBUG==1) check_error(size);
    /* Get Message */
    char client_message[BUFSIZ];
    ssize_t read_size;
    /*
    if ((read_size = recv(sock,client_message,BUFSIZ,0)) > 0){
	strcat(cat,client_message);
    }*/
    bzero(client_message, BUFSIZ);
    recv(sock,client_message,BUFSIZ,0);
    printf("Broadcast mssg: %s\n", client_message);
    strcpy(message, client_message);
    if (DEBUG==1) check_error(read_size);
}

void read_it(queue <char *> * _messages, int sock){	
    queue<char *> messages = *_messages;
    char * to_send = messages.back();
    messages.pop();
    /* Send Probe */
    ssize_t size = send(sock,to_send,sizeof(to_send),0);
    //bzero(cat,sizeof(cat));
    if (DEBUG==1) check_error(size); 
}

char * format_msg(char * user,int mode,char *data,map<char *,queue<char *> *> dict){
     char *formatted_str;
     formatted_str = (char *)malloc(BUFSIZ*4);
     strcpy(formatted_str, ""); 
     if (mode==0){ // Message Handler
           strcat(formatted_str,"C");
	   strcat(formatted_str,",");
           strcat(formatted_str,user);
           strcat(formatted_str,",");
           strcat(formatted_str,data); 
     }else if (mode==1){ // List handler
	   strcat(formatted_str,"C");
           map<char *,queue<char *> *>::iterator it;
           for (it=dict.begin(); it !=dict.end();it++){
		if (it->second!=NULL){
           		strcat(formatted_str,",");
           		strcat(formatted_str,it->first);
	        }
	   } 
     }
     return formatted_str;
}


void write_it(char client_message_input, char * user, map<char *,queue<char *> *> * dict, int sfd) {
    /* Inits */
    int sock = sfd;
    ssize_t read_size;
    char message[BUFSIZ];
    char client_message[BUFSIZ];
    char recipient[BUFSIZ];
    char formatted_buf[BUFSIZ];
    char cat_buf[BUFSIZ];

    get_message(sock,1,cat_buf); // Puts formatted mssg in cat_buf 
    // Parse username from message
    printf("mssg: %s\n", cat_buf);
    char * tok = strtok(cat_buf, ",");
    user = strtok(NULL, ",");
    strcpy(message, strtok(NULL, ","));
    printf("user: %s\n", user);
    /* Read Message Type */
   
    printf("switch: %c\n",client_message_input);
    switch(client_message_input){
       case 'B': /* Broadcast Message */
	 printf("message: %s\n", message);
	 printf("dict[user]: %p\n", (*dict)[user]);
	 printf("ben: %p\n", (*dict)["Ben"]);
           for (int i = 0; i < MAX_SIZE; i++){
	      printf("%d\n", i);
              if (&qs[i] != (queue<char *> *)((*dict)[user]) && in_use[i]){
		strcpy(formatted_buf, format_msg(user,0,cat_buf,*dict));
		printf("formatted buf: %s, user: %s\n", formatted_buf, user);
		qs[i].push(formatted_buf);
	      }
	   } 
	   break;
       case 'L': /* Personal Message */
	 strcpy(format_msg(user,1,NULL,*dict), formatted_buf);
    	   read_size = send(sock,formatted_buf,sizeof(formatted_buf),0);
       case 'P': /* Personal Message */
	   get_message(sock,0, recipient);
	   get_message(sock,1, cat_buf);
           for (int i = 0; i < MAX_SIZE; i++){
              if (&qs[i]==(*dict)[recipient]){
		strcpy(format_msg(user,0,cat_buf,*dict), formatted_buf);
		  qs[i].push(formatted_buf);
	      }
	   }
           break;

       case 'E': /* Cleanup Message */
           fprintf(stdout,"Peace fam\n");	   
           (*dict)[user] = NULL;
           break;

       default:
 	   break;
    }
    close(sock); 
}

void check_error(ssize_t size){
    /* Check for Empty Channel */
    if(size == 0){
          fprintf(stdout,"Client Disconnected");
          fflush(stdout);
    /* Check Failure */
    }else if(size == -1){
          fprintf(stderr,"[Recv or Send] : %s",strerror(errno));
    }    
 
}
