# Using g++ compliler
# suppressing compiler warnings
CC=g++
CCFLAGS=-w
SOURCES=main.cpp ftpserverfunctions.cpp
DEPS = ftpserverfunctions.h
OBJ = main.o ftpserverfunctions.o
TARGET=ftpserver

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(CCFLAGS) $(SOURCES) -o $(TARGET)

main.o: main.cpp
	$(CC) $(CCFLAGS) -c main.cpp

ftpserverfunctions.o: ftpserverfunctions.cpp
	$(CC) $(CCFLAGS) -c ftpserverfunctions.cpp

.PHONY: clean

clean:
	rm *.o ftpserver