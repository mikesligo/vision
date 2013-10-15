CC=clang++
CFLAGS = `pkg-config --cflags opencv`
LIBS = `pkg-config --libs opencv`
SOURCES = lab1.cpp

all: 
	$(CC) $(CFLAGS) $(LIBS) $(SOURCES) -o lab1
