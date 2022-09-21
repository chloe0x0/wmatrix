CC = gcc
TARGET = wmatrix
CFLAGS = -Wall -O2 -o $(TARGET)
DEL_CMD = del 	# rm on linux

all:
	$(CC) $(CFLAGS) ./src/wmatrix.c
clean:
	$(DEL_CMD) *.exe
