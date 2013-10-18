CC=g++
CFLAGS = `pkg-config --cflags opencv` -Wall -g
LIBS = `pkg-config --libs opencv`
SOURCES = lab1.cpp

all: 
	$(CC) $(SOURCES) $(CFLAGS) $(LIBS)  -o lab1
