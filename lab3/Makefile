CC=g++
CFLAGS = `pkg-config --cflags opencv` -Wall -g
LIBS = `pkg-config --libs opencv`
SOURCES = flann.cpp

all: 
	$(CC) $(SOURCES) $(CFLAGS) $(LIBS)  -o exe
