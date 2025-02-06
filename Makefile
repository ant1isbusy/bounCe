# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c11
LDFLAGS = `pkg-config --cflags --libs sdl2` -lm -lSDL2_gfx

TARGET = bounce
SRC = bounce.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

# Run the program
run: $(TARGET)
	./$(TARGET)

# Clean up build files
clean:
	rm -f $(TARGET)

# Phony targets
.PHONY: all run clean
