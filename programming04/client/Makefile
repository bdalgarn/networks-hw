CXX             = g++
CXXFLAGS        = -g -gdwarf-2 -std=gnu++11 -Wall 
LD              = g++
LDFLAGS         = -Llib
LNKFLAGS        = -lpthread


# These are macros to build the client static library.
CLIENT_SOURCE   = $(wildcard *.cpp)
CLIENT_OBJECTS  = $(CLIENT_SOURCE:.cpp=.o)
CLIENT          = libps_client.a

TARGETS         = $(CLIENT)

all:            $(TARGETS)

$(CLIENT):      $(CLIENT_OBJECTS)
	$(AR) $(ARFLAGS) $@ $^

client/%.o: src/client/%.cpp $(wildcard ../include/ps_client/*.h)
	$(CXX) $(CXXFLAGS) -o $@ -c $<

clean:
	rm -fr build $(TARGETS) $(CLIENT_OBJECTS)

.PHONY: all clean

