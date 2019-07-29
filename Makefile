BUILD_DIR=../build/temp.linux-x86_64-3.6
CC=g++
CFLAGS = -I/usr/local/include -Wall -std=c++17 -O3 -fPIC  -march=native -Ofast -ftree-vectorize
LIBS=-shared -pthread -lGeographic
SOURCES=$(sort $(wildcard *.cpp))
OBJECTS=$(SOURCES:.cpp=.o)
OBJECTS2=$(BUILD_DIR)/vptree.o
TARGET=vptree.cpython-36m-x86_64-linux-gnu.so


all:$(TARGET)
	install $(TARGET) ../

$(TARGET) : $(OBJECTS) 
	$(CC)  $(LIBS)  -o $(TARGET) $(OBJECTS2) $(OBJECTS)


$(OBJECTS): %.o: %.cpp
	$(CC) -c -g $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

