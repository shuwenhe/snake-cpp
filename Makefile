CC = gcc
CFLAGS = -lncurses
TARGET = snake
SRC = snake.c

.PHONY: all run clean

all: run

run: $(TARGET)
	./$(TARGET)

$(TARGET): $(SRC)
	$(CC) -o $(TARGET) $(SRC) $(CFLAGS)

clean:
	rm -f $(TARGET)
