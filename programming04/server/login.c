#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>

#define USERS_FILE_NAME "users.txt"
#define MAX_USERNAME_LENGTH 64
#define MAX_PASSWORD_LENGTH 64

void loginUser(const char * username);
void handleExistingUser(const char * username);
void handleNewUser(const char * username);

int main() {
	loginUser("Chris");		
}

void loginUser(const char * username) {
	char nameFromFile[MAX_USERNAME_LENGTH + 1];
	char passwordFromFile[MAX_PASSWORD_LENGTH + 1];
	FILE *usersFile = fopen(USERS_FILE_NAME, "r+");
	int found = 0;
	
	if ( usersFile != NULL ) {
		while ( fscanf(usersFile, "%s %s", nameFromFile, passwordFromFile) == 2 ) {
			if ( strncmp(nameFromFile, username, MAX_USERNAME_LENGTH) == 0 ) {
				fclose(usersFile);
				usersFile = NULL;

				handleExistingUser(username);
				found = 1;
				break;
			}
		}	
	}
	if ( !found ) {
		fclose(usersFile);
		usersFile = NULL;

		handleNewUser(username);
	}	
}

void handleExistingUser(const char * username) {
	/* 
	 * send and recv password info and verify			
	 */
}

void handleNewUser(const char * username) {
	/*
	 * send and recv password info
	 */
	char *password = "password"; //TODO: put an actual value in this array

	FILE *usersFile = fopen(USERS_FILE_NAME, "a");
	char newEntry[MAX_USERNAME_LENGTH + MAX_PASSWORD_LENGTH + 1];
	sprintf(newEntry, "%s %s\n", username, password);
	printf("%s\n", newEntry);
	fputs(newEntry, usersFile);
	fclose(usersFile);
}
