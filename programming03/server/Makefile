CC=             g++
CFLAGS=             -ggdb -Wall -std=gnu++11
LD=             g++
LDFLAGS=
AR=             ar
ARFLAGS=        rcs
TARGETS=                      	server.o\
                                myftpd.o\
				myftpd

all:            $(TARGETS)

myftpd: myftpd.o
	@echo "Linking $@..."
	@$(LD) $(LFLAGS) -o myftpd myftpd.o server.o

myftpd.o: myftpd.cpp
	@echo "Compiling $@..."
	@$(CC) $(CFLAGS) -c -o myftpd.o myftpd.cpp

server.o: server.cpp
	@echo "Compiling $@..."
	@$(CC) $(CFLAGS) -c -o server.o server.cpp

clean:
	@echo Cleaning...
	@rm -f $(TARGETS) *.o *.log *.input

.PHONY:         all test benchmark clean
