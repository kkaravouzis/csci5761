# Using g++ compliler
# suppressing compiler warnings
CC=g++
CCFLAGS=-w
SOURCES=mainc.cpp ftpclientfunctions.cpp
DEPS = ftpclientfunctions.h
OBJ = mainc.o ftpclientfunctions.o
TARGET=ftpclient

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(CCFLAGS) $(SOURCES) -o $(TARGET)

main.o: mainc.cpp
	$(CC) $(CCFLAGS) -c mainc.cpp

ftpserverfunctions.o: ftpclientfunctions.cpp
	$(CC) $(CCFLAGS) -c ftpclientfunctions.cpp

.PHONY: clean

clean:
	rm $(OBJ) $(TARGET)