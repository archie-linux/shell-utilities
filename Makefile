# Makefile

# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -pedantic

# List of source files (replace with your actual file names)
SRCS = my-cat.c # Add your C files here

# Default target
all: $(SRCS:.c=)

# Rule for compiling .c files to executables
%: %.c
	$(CC) $(CFLAGS) $< -o $@

# Clean target
clean:
	rm -f $(SRCS:.c=)

.PHONY: all clean
