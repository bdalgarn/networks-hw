// Brianna Hoelting, Anthony DiFalco, Ben Dalgarn
// bhoeltin, adifalco, bdalgarn

/*


Then the socket must actually be activated. The  then must bind to its address and wait for a client to connect. Once the server reciev\
es the first message from a client, it will send back the key. Then the user will send a message of text to the server which the server will \
then encrypt and send back to the client. Then the server will wait for another client to connect and repeat the process.


*/

#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <sys/time.h>

using namespace std;

class server {

  public:
    server(int);
    ~server();
    int dwld(string, int);
    int upld(string, int);
    int delf(string, int);
    int list();
    int mdir(string, int);
    int rdir(string, int);
    int cdir(string, int);
    int quit();

  private:
    int portNumber, s;
    struct sockaddr_in sin;

};

#endif
