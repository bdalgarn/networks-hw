#include <stdint.h>


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

