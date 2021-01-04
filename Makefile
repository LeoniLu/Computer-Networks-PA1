CC = gcc
CXX= g++

INCLUDES =
CFLAGS = -g -Wall $(INCLUDES)
CXXFLAGS = -g -Wall $(INCLUDES)

LDFLAGS = -g
LDLIBS =
.PHONY:default
default:client server UdpChat

UdpChat:

UdpChat.o:func.h

client:client.o func.o

client.o:func.h

server:server.o func.o

server.o:func.h

func.o:func.h

.PHONY: clean
clean:
	rm -f *.o a.out core client server UdpChat 

.PHONY: all
all: clean client server UdpChat
