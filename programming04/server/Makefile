CC=             g++
CFLAGS=             -ggdb -Wall -std=gnu++11
LD=             g++
LDFLAGS=
AR=             ar
ARFLAGS=        rcs
TARGETS=	server                      	

all:            $(TARGETS)

server: server.cpp
	@echo "Linking $@..."
	g++ -pthread -std=gnu++11 -ggdb -o chatserver server.cpp

tmp: server_tmp.cpp
	@echo "Linking $@..."
	g++ -pthread -std=c++0x -ggdb -o chatserver_tmp server_tmp.cpp


clean:
	@echo Cleaning...
	@rm -f $(TARGETS) *.o *.log *.input chatserver_tmp chatserver
