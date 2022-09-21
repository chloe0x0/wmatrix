CC = gcc
TARGET = matrix
CFLAGS = -Wall -O2 -o $(TARGET)
DEL_CMD = del 	# rm on linux

all:
	$(CC) $(CFLAGS) ./src/matrix.c
clean:
	$(DEL_CMD) *.exe
