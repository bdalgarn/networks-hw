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

int loginUser(const char * username);
void handleExistingUser(const char * username);
void handleNewUser(const char * username, const char * password);


// Returns 0 if a new user, 1 if an existing user
int loginUser(const char * username, char * password) {
	char nameFromFile[MAX_USERNAME_LENGTH + 1];
	char passwordFromFile[MAX_PASSWORD_LENGTH + 1];
	FILE *usersFile = fopen(USERS_FILE_NAME, "r+");

	int found = 0;
	while ( fscanf(usersFile, "%s %s", nameFromFile, passwordFromFile) == 2 ) {
		if ( strncmp(nameFromFile, username, MAX_USERNAME_LENGTH) == 0 ) {
			fclose(usersFile);
			usersFile = NULL;

			strcpy(password, passwordFromFile);
			found = 1;
			break;
		}
	}	
	if ( !found ) {
		fclose(usersFile);
		usersFile = NULL;

		password = NULL;
		return 0;
	}	
	return 1;
}

void handleExistingUser(const char * username) {
	/* 
	 * send and recv password info and verify			
	 */
}

void handleNewUser(const char * username, const char * password) {
	/*
	 * send and recv password info
	 */

	FILE *usersFile = fopen(USERS_FILE_NAME, "a");
	char entry[MAX_USERNAME_LENGTH + MAX_PASSWORD_LENGTH + 1];
	sprintf(entry, "%s %s", username, password);
	printf("entry: %s\n", entry);
	int ret = fputs(entry, usersFile);
	fclose(usersFile);
}
