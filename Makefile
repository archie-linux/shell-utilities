# Makefile

# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -pedantic

# List of source files
SRCS = cat.c find.c

# Default target
all: $(SRCS:.c=)

# Rule for compiling .c files to executables
%: %.c
	$(CC) $(CFLAGS) $< -o $@

# Clean target
clean:
	rm -f $(SRCS:.c=)

.PHONY: all clean
