#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdbool.h>

#define MAX_LINES 1000
#define BUFFER_SIZE 1024

void print_last_n_lines(FILE *file, int n, const char *filename) {
    char *lines[MAX_LINES];  // Circular buffer
    int count = 0;           // Number of lines read
    int index = 0;           // Current index for the circular buffer

    if (filename != NULL) {
        printf("==> %s <==\n", filename);  // Print filename header for multiple files
    }

    // Initialize the buffer
    for (int i = 0; i < MAX_LINES; i++) {
        lines[i] = malloc(BUFFER_SIZE);
    }

    // Read lines into the circular buffer until EOF
    while (!feof(file)) {
        if (fgets(lines[index], BUFFER_SIZE, file) != NULL) {
            index = (index + 1) % MAX_LINES; // Move to the next index (circular)
            count++;
        }
    }

    for (int i = count - n; i < count; i++) {
        printf("%s", lines[i]);
    }

    printf("\n");

    // Deallocate memory
    for (int i = 0; i < MAX_LINES; i++) {
        free(lines[i]);
    }
}

void print_last_n_bytes(FILE *file, int bytes, const char *filename) {
    if (filename != NULL) {
        printf("==> %s <==\n", filename);  // Print filename header for multiple files
    }

    fseek(file, -bytes, SEEK_END);
    char *buffer = malloc(bytes + 1);

    fread(buffer, 1, bytes, file);
    buffer[bytes] = '\0'; // Null-terminate the buffer

    printf("%s", buffer);
    free(buffer); // Free allocated memory
}

void follow_file(FILE *file) {
    fseek(file, 0, SEEK_END); // Move to the end of the file
    while (true) {
        char buffer[BUFFER_SIZE];

        // Read a line from the file
        if (fgets(buffer, BUFFER_SIZE, file) != NULL) {
            printf("%s", buffer);
        } else {
            // If fgets returns NULL, check for EOF
            if (feof(file)) {
                // Reset the file pointer to the end again
                fseek(file, 0, SEEK_END);
            }
            usleep(100000); // Sleep for 0.1 second
        }
    }
    fclose(file);
}

void process_file(const char *filename, int lines, int bytes, bool follow) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return;
    }

    if (bytes > 0) {
        print_last_n_bytes(file, bytes, filename);
    } else if (lines > 0) {
        print_last_n_lines(file, lines, filename);
    }

    if (follow) {
        follow_file(file);
    }

    fclose(file);
}

void process_stdin(int lines, int bytes, bool follow) {
    if (bytes > 0) {
        print_last_n_bytes(stdin, bytes, NULL);
    } else {
        print_last_n_lines(stdin, lines, NULL);
    }

    if (follow) {
        follow_file(stdin);
    }
}

int main(int argc, char *argv[]) {
    int lines = 10; // Default number of lines
    int bytes = 0;  // Default number of bytes
    bool follow = false;

    int opt;
    while ((opt = getopt(argc, argv, "n:c:f")) != -1) {
        switch (opt) {
            case 'n':
                lines = atoi(optarg);
                break;
            case 'c':
                bytes = atoi(optarg);
                break;
            case 'f':
                follow = true;
                break;
            default:
                fprintf(stderr, "Usage: %s [-n lines] [-c bytes] [-f] [file...]\n", argv[0]);
                exit(EXIT_SUCCESS);
        }
    }

    if (optind >= argc) {
        // If no files are provided, read from stdin
        process_stdin(lines, bytes, follow);
    }

    // Process each provided file
    for (int i = optind; i < argc; i++) {
        process_file(argv[i], lines, bytes, follow);
    }

    return 0;
}
