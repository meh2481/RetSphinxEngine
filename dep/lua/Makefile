# Simple makefile for building lua as static lib
OBJECTS = $(shell ls -1 *.c | sed 's/\.c/\.o/')
TARGET = liblua.a
CFLAGS = 
CC = cc

all: $(TARGET)

%.o: %.c
	$(CC) -g -c -o $@ $< $(CFLAGS)

clean: clean-obj clean-lib

clean-obj:
	rm -rf *.o
	
clean-lib:
	rm -rf $(TARGET)
	
$(TARGET): $(OBJECTS)
	ar rcs $(TARGET) $(OBJECTS)