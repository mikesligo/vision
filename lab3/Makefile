CC=g++
CFLAGS = `pkg-config --cflags opencv` -Wall -g
LIBS = `pkg-config --libs opencv`
SOURCES = hi.cpp

all: 
	$(CC) $(SOURCES) $(CFLAGS) $(LIBS)  -o exe
