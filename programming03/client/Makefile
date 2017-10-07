CC=             g++
CFLAGS=             -ggdb -Wall -std=gnu++11
LD=             g++
LDFLAGS=
AR=             ar
ARFLAGS=        rcs
TARGETS=                        myftp\
                                myftp.o

all:            $(TARGETS)

myftp: myftp.o
	@echo "Linking $@..."
	@$(LD) $(LFLAGS) -o myftp myftp.o

myftp.o: FTPcli.cpp
	@echo "Compiling $@..."
	@$(CC) $(CFLAGS) -c -o myftp.o FTPcli.cpp

clean:
	@echo Cleaning...
	@rm -f $(TARGETS) *.o *.log *.input

.PHONY:         all test benchmark clean


