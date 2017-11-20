#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#define MAX_SIZE 10
#define DEBUG 1
using namespace std;



void *connection_handler(void *);

map<char *,queue *> queues;

//queue <queue<char *>> qs[10];
//bzero(qs,sizeof(queue)*sizeof(char *));


int main(int argc , char *argv[]){

    /* Initializations */
    queue <char *> qs[10];
    bzero(qs,sizeof(queue)*sizeof(char *));
    int sock , client_sock , c , *new_sock;
    struct sockaddr_in server , client;
    bzero((char *)&server,sizeof(server));
    bzero((char *)&client,sizeof(client));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 8888 );
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
        new_sock = malloc(1);    // Allocate space
        *new_sock = client_sock; // Duplicae address

       char * user_name = "ayy lmao";
       struct args {
		queue <char *> user_qs[10] = &qs;
		char * user = user_name;	
		map <char *, queue <char *> *> user_map = &users; 		   
		int sock = new_sock;
       };
        /* Creates New Thread */
        if(pthread_create(&client_thread,NULL,connection_handler,(void*)args)<0){
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





void *connection_handler(void *args){
    /* Initializations */
    int sock = *(int*)args->sock;
    char * to_be_written,user;  // This is going to be what we need to write to the users queue
    char * to_be_recieved; // This is what we need to send to our client (messages from others in their queue)
    ssize_t read_size;
    char *message , client_message[BUFSIZ];
    bool mode = false; // This value is going to determine whether to write to the queues of read from them
		       // Kinda lost about ideas on that tho so pls add whatever makes sense to you

    /* Casts vals back from void */
    queue <char *> copy_q[10] = (queue <char *>)args->user_qs;
    char * user_name = (char *)args->user;
    map<char *,queue *> copy_map = (map<char *,queue *> *)args->user_map;
    queue <char *> user_q = &(queue <char *>)copy_map[user_name];

    if(mode){
	    for (int i = 0; i < MAX_SIZE; i++){
		 if (&copy_q[i]==user_q){
		   read_it(*user_q,(int*)args->sock);
		}
	    }
    }else{    
	   write_it(copy_q,user_name,&opy_map,(int*)args->sock);
    }
}



char * get_mesage(int *sock, int mode){
    /* Build Probe */
    char * buf;
    if (mode==1){
       buf = "P,Server,Please Enter Your Message : ";
    }else if (mode==0){
       buf = "P,Server,Please Enter Recipient : ";
    }else{
       return NULL;
    }
    /* Send Probe */
    ssize_t size = send(sock,buf,sizeof(buf),0);
    bzero(cat,sizeof(cat));
    if (DEBUG==1) checkerror(size);
    /* Get Message */
    char *client_message[BUFSIZ];
    char *cat[BUFSIZ*4];
    while((read_size = recv(sock,client_message,BUFSIZ,0))>0){
	strcat(cat,client_message);
    }
    if (DEBUG==1) check_error(read_size);
    return cat;
}

void read_it(queue <char *> &messages, int sock){	
    char * to_send = messages.pop();
    /* Send Probe */
    ssize_t size = send(sock,to_send,sizeof(to_send),0);
    bzero(cat,sizeof(cat));
    if (DEBUG==1) checkerror(size); 
}

char * format_msg(char * user,int mode,char *data,map<char *,queue *> dict){
     char *formatted_str[BUFSIZ*4];
     if (mode==0){ // Message Handler
           strcat(formatted_str,'C');
	   strcat(formatted_str,',');
           strcat(formatted_str,user);
           strcat(formatted_str,',');
           strcat(formatted_str,data); 
     }else if (mode==1){ // List handler
	   strcat(formatted_str,'C');
           map<char *,queue *>::iterator it;
           for (it=dict.begin(); it !=dict.end();it++){
		if (it->second!=NULL){
           		strcat(formatted_str,',');
           		strcat(formatted_str,it->first);
	        }
	   } 
     }
     return formatted_str;
}


void write_it(queue <char *>  qs, char * user, map<char *,queue *> dict, int *sfd)
    /* Inits */
    int sock = *sfd;
    ssize_t read_size;
    char *message, client_message[BUFSIZ], recipient[BUFSIZ];
    char *cat_buf[BUFSIZ*4],formatted_buf[BUFSIZ*4];

    /* Read Message Type */
    read_size = recv(sock ,client_message,BUFSIZ,0));
    if (DUBUG==1) checkerror(read_size);
    bzero(client_message,sizeof(client_message));

    switch(client_message[0]){
       case 'B': /* Broadcast Message */
           cat_buf = get_message(sock,1); 
           for (int i = 0; i < MAX_SIZE; i++){
              if (&qs[i]!=&dict[user]){
                  formatted_buf = format_msg(user,0,cat_buf,dict);
		  qs[i].push(formatted_buf);
	      }
	   } 
	   break;
       case 'L': /* Personal Message */
	   formatted_buf = format_msg(user,1,NULL,dict);
    	   read_size = send(sock,formatted_buf,sizeof(formatted_buf),0);
       case 'P': /* Personal Message */
           recipient = get_message(sock,0);
	   cat_buf = get_message(sock,1);
           for (int i = 0; i < MAX_SIZE; i++){
              if (&qs[i]==&dict[recipient]){
                  formatted_buf = format_msg(user,0,cat_buf,dict){
		  qs[i].push(formatted_buf);
	      }
	   }
           break;

       case 'E': /* Cleanup Message */
           fprintf(stdout,"Peace fam\n");	   
           dict[user] = NULL;
           break;

       default:
 	   break;
    }
    free(sock); 
    return 0;
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
