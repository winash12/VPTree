CC=g++
CFLAGS = -I/usr/local/include -Wall -std=c++17 -O3 -fPIC  -march=native -Ofast -ftree-vectorize
LIBS=-shared -pthread 
SOURCES=$(sort $(wildcard *.cpp))
OBJECTS=$(SOURCES:.cpp=.o)
TARGET=vptree.cpython-36m-x86_64-linux-gnu.so


all:$(TARGET)


$(TARGET) : $(OBJECTS)
	$(CC)  $(LIBS)  -o $(TARGET)  $(OBJECTS)


$(OBJECTS): %.o: %.cpp
	$(CC) -c -g $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

