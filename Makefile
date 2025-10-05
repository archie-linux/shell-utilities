# Makefile

# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -Wno-strict-prototypes -pedantic

# List of source files
SRCS = cat.c find.c head.c

# Default target
all: $(SRCS:.c=)

# Rule for compiling .c files to executables
%: %.c
	$(CC) $(CFLAGS) $< -o $@

# Clean target
clean:
	rm -f $(SRCS:.c=)

.PHONY: all clean
