CC=		g++
CFLAGS=		 -ggdb  -Wall -std=gnu++11
LD=		g++
LDFLAGS=	-L.
TARGETS=	chatclient\
		main.o\
		client.o\
		thread.o

all:		$(TARGETS)

chatclient: main.o thread.o client.o 
	@echo "Linking $@..."
	@$(LD) $(LDFLAGS) -lpthread -o chatclient main.o client.o thread.o

main.o: main.cpp client.h thread.h
	$(CXX) $(CXXFLAGS) -c main.cpp

client.o:	client.cpp
	@echo "Compiling $@..."
	@$(CC) $(CFLAGS) -c -o client.o client.cpp

thread.o:       thread.cpp
	@echo "Compiling $@..."
	@$(CC) $(CFLAGS) -c -o thread.o thread.cpp

clean: 
	@echo Cleaning...
	@rm -f $(TARGETS) *.o *.log *.input

.PHONY:		all clean

