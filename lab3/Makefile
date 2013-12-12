CC=g++
CFLAGS = `pkg-config --cflags opencv` -Wall -g 
LIBS = `pkg-config --libs opencv`
SOURCES = lab3.cpp

all: 
	$(CC) $(SOURCES) $(CFLAGS) $(LIBS)  -o exe
