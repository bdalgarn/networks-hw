#include <stdint.h>


void mdir(char *name, int32_t size){
    char buf[MAXSIZE];
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
    /* Sends Message */
    if ((sendto(new_s,buf,sizeof(buf),0,(struct sockaddr *)&new_s,sizepf(struct sockaddr)))<0){
	fprintf(stderr,"[MDIR sendto #1] : %s",strerror(errno));
	continue;
    }
}




void rdir(char *name, int32_t size){
    char buf[MAXSIZE];
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
    /* Sends Message */
    if ((sendto(new_s,buf,sizeof(buf),0,(struct sockaddr *)&new_s,sizepf(struct sockaddr)))<0){
	fprintf(stderr,"[MDIR sendto #1] : %s",strerror(errno));
	continue;
    }

}


void cdir(char *name, int32_t size){
    char buf[MAXSIZE];
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
    /* Sends Message */
    if ((sendto(new_s,buf,sizeof(buf),0,(struct sockaddr *)&new_s,sizepf(struct sockaddr)))<0){
	fprintf(stderr,"[MDIR sendto #1] : %s",strerror(errno));
	continue;
    }

}

