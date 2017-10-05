CC=             g++
CFLAGS=             -ggdb -Wall -std=gnu++11
LD=             g++
LDFLAGS=
AR=             ar
ARFLAGS=        rcs
TARGETS=                        myftpd\
                                myftpd.o

all:            $(TARGETS)

myftpd: myftpd.o
	@echo "Linking $@..."
	@$(LD) $(LFLAGS) -o myftpd myftpd.o

myftpd.o: myftpd.cpp
	@echo "Compiling $@..."
	@$(CC) $(CFLAGS) -c -o myftpd.o myftpd.cpp

clean:
	@echo Cleaning...
	@rm -f $(TARGETS) *.o *.log *.input

.PHONY:         all test benchmark clean
