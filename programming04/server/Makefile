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
	g++ -pthread -ggdb -Wall -std=gnu++11 -o server server.cpp

clean:
	@echo Cleaning...
	@rm -f $(TARGETS) *.o *.log *.input
