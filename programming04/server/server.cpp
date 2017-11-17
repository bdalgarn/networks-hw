#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>



void *connection_handler(void *);

int main(int argc , char *argv[]){
    /* Initializations */
    int sock , client_sock , c , *new_sock;
    struct sockaddr_in server , client;
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

	/* Here is where we are going to have to open up sockets based
 *      on a sort of ACK-oriented policy handlers.
 *
 *      1. Open up UDP port
 *      2. Recieve client's initial ""
 *
 */

        new_sock = malloc(1);    // Allocate space
        *new_sock = client_sock; // Duplicae address

        /* Creates New Thread */
        if(pthread_create(&client_thread,NULL,connection_handler,(void*)new_sock)<0){
            perror("could not create thread");
            return 1;
        }
     }
    /* Check Error */
B
    if (client_sock < 0){
        perror("accept failed");
        return 1;
    }
    close(sock);
    return 0;
} 










void *connection_handler(void *sock){
    /* Initializations */
    int sock = *(int*)sock;
    ssize_t read_size;
    char *message , client_message[2000];
    /*  Read Input and  Write to Socket*/ 
    while((read_size = recv(sock ,client_message,2000,0))>0){
          write(sock , client_message , strlen(client_message));
    }
    /* Check for Empty Channel */
    if(read_size == 0){
          puts("Client disconnected");
          fflush(stdout);
    /* Check Failure */
    }else if(read_size == -1){
          perror("recv failed");
    }    
    free(sock); // Free mem
    return 0;
}


