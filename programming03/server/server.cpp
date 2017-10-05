// Brianna Hoelting, Anthony DiFalco, Ben Dalgarn
// bhoeltin, adifalco, bdalgarn

/*


Then the socket must actually be activated. The server then must bind to its address and wait for a client to connect. Once the server reciev\
es the first message from a client, it will send back the key. Then the user will send a message of text to the server which the server will \
then encrypt and send back to the client. Then the server will wait for another client to connect and repeat the process.


*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <sys/time.h>
#include "server.h"

using namespace std;


server::server(int portNum){

  portNumber = portNum;

  int opt = 1;

  bzero((char *)&sin, sizeof(sin));
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = htonl(INADDR_ANY); //Use the default IP address of server
  sin.sin_port = htons(portNumber);

  if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("simplex-talk:	socket");
    exit(1);
  }

  if	((setsockopt(s,	SOL_SOCKET,	SO_REUSEADDR,	(char	*)&	opt,	sizeof(int)))<0){
    perror	("simplex-talk:setscokt");
    exit(1);
  }

  if	((bind(s,	(struct sockaddr	*)&sin,	sizeof(sin)))	<	0)	{
    perror("simplex-talk:	bind");
    exit(1);
  }

  if	((listen(s,	10))<0){
    perror("simplex-talk:	listen");
    exit(1);
  }

}

server::~server(){}

int server::dwld(string fileName, int nameLen){

  
}


int server::upld(string fileName, int nameLen){

}

int server::delf(string fileName, int nameLen){

}


int server::list(){

}
int server::mdir(string dirName, int nameLen){

}

int server::rdir(string dirName, int nameLen){

}

int server::cdir(string dirName, int nameLen){

}

int server::quit(){

}
