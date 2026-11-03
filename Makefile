CC = g++
CFLAGS = -lncurses
TARGET = snake
SRC = snake.cpp

.PHONY: all run clean

all: run

run: $(TARGET)
	./$(TARGET)

$(TARGET): $(SRC)
	$(CC) -o $(TARGET) $(SRC) $(CFLAGS)

clean:
	rm -f $(TARGET)
