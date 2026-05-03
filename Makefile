CC = gcc
CFLAGS = -std=c11 -O2 -Wall -Wextra -Iinclude
LDFLAGS =
OBJ = src/main.o src/algorithms.o src/io.o
TARGET = jackfruit

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LDFLAGS)

src/main.o: src/main.c include/scheduler.h include/algorithms.h
src/algorithms.o: src/algorithms.c include/scheduler.h include/algorithms.h
src/io.o: src/io.c include/scheduler.h

clean:
	rm -f $(TARGET) $(OBJ)

.PHONY: all clean
