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


int main(int argc, char * argv[]){

  if (argc != 2) exit(1);

  int port_number = atoi(argv[1]);

  server server(port_number);


}
