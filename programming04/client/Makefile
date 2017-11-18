CXX             = g++
CXXFLAGS        = -g -gdwarf-2 -std=gnu++11 -Wall
LD              = g++
LDFLAGS         = -Llib
LNKFLAGS        = -lpthread


# These are macros to build the client static library.
CLIENT_SOURCE   = $(wildcard *.cpp)
CLIENT_OBJECTS  = $(CLIENT_SOURCE:.cpp=.o)
CLIENT          = lib/libps_client.a

CHAT_LNK_FLAGS   = $(LNKFLAGS) -lps_client
CHAT_SOURCE      = main.cpp
CHAT_OBJECTS     = $(CHAT_SOURCE:.cpp=.o)
CHAT             = chatclient


TARGETS         = $(CLIENT) $(CHAT)

all:            $(TARGETS)

$(CLIENT):      $(CLIENT_OBJECTS)
	$(AR) $(ARFLAGS) $@ $^

client/%.o: %.cpp $(wildcard ps_client/*.h)
	$(CXX) $(CXXFLAGS) -o $@ -c $<

$(CHAT):         $(CHAT_OBJECTS) $(CLIENT)
	$(LD) -o $@ $(CHAT_OBJECTS) $(LDFLAGS) $(CHAT_LNK_FLAGS)

main.o:    main.cpp $(wildcard ps_client/*.h)
	$(CXX) $(CXXFLAGS) -o $@ -c $<

clean:
	rm -fr build $(TARGETS) $(CLIENT_OBJECTS) $(CHAT_OBJECTS)

.PHONY: all clean

