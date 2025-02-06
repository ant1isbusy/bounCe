# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c11
LDFLAGS = `pkg-config --cflags --libs sdl2` -lm -lSDL2_gfx

TARGET = bounce

SRC = bounce.c
OBJ = $(SRC:.c=.o)

all: bin

bin: $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) $(LDFLAGS)

# Compile individual source files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Run the program
run: bin
	./$(TARGET)

# Clean up build files
clean:
	rm -f $(OBJ) $(TARGET)

# Phony targets (not actual files)
.PHONY: all bin run clean
