#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_LINES 10

// Function to display usage information
void usage() {
    printf("Usage: head_program [OPTION]... [FILE]...\n");
    printf("Print the first 10 lines of each FILE to standard output.\n");
    printf("With more than one FILE, precede each with a header giving the file name.\n\n");
    printf("Options:\n");
    printf("  -n NUM    print the first NUM lines instead of the default 10\n");
    printf("  -c NUM    print the first NUM bytes of each file\n");
    printf("  -h        display this help and exit\n\n");
    printf("With no FILE, or when FILE is -, read standard input.\n");
    exit(EXIT_SUCCESS);
}

void print_head_lines(FILE *file, int num_lines, const char *filename) {
    if (filename != NULL) {
        printf("==> %s <==\n", filename);  // Print filename header for multiple files
    }

    char buffer[1024];
    int count = 0;
    while (fgets(buffer, sizeof(buffer), file) != NULL && count < num_lines) {
        printf("%s", buffer);
        count++;
    }
    printf("\n");
}

void print_head_bytes(FILE *file, int num_bytes, const char *filename) {
    if (filename != NULL) {
        printf("==> %s <==\n", filename);  // Print filename header for multiple files
    }

    char buffer[1];
    int count = 0;
    while (fread(buffer, 1, 1, file) == 1 && count < num_bytes) {
        printf("%c", buffer[0]);
        count++;
    }
    printf("\n");
}

void process_file(const char *filename, int num_lines, int num_bytes, int is_bytes) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    if (is_bytes) {
        print_head_bytes(file, num_bytes, filename);
    } else {
        print_head_lines(file, num_lines, filename);
    }

    fclose(file);
}

void process_stdin(int num_lines, int num_bytes, int is_bytes) {
    if (is_bytes) {
        print_head_bytes(stdin, num_bytes, NULL);
    } else {
        print_head_lines(stdin, num_lines, NULL);
    }
}

int main(int argc, char *argv[]) {
    int num_lines = DEFAULT_LINES;
    int num_bytes = -1;
    int is_bytes = 0;  // Flag to indicate if -c (bytes) is used
    int file_start = 1;
    
    if (argc < 2) usage();

    // Parse command-line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-n") == 0 && i + 1 < argc) {
            num_lines = atoi(argv[++i]);
            file_start = i + 1;
        } else if (strcmp(argv[i], "-c") == 0 && i + 1 < argc) {
            num_bytes = atoi(argv[++i]);
            is_bytes = 1;
            file_start = i + 1;
        } else if (strcmp(argv[i], "-h") == 0) {
            usage();
            return 0;
        }
    }

    // Check if any files are provided
    if (file_start < argc) {
        // Multiple files case
        for (int i = file_start; i < argc; i++) {
            if (is_bytes) {
                process_file(argv[i], num_lines, num_bytes, is_bytes);
            } else {
                process_file(argv[i], num_lines, num_bytes, is_bytes);
            }
        }
    } else {
        // No files provided, read from stdin
        process_stdin(num_lines, num_bytes, is_bytes);
    }

    return 0;
}
