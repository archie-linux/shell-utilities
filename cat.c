#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define BUFFER_SIZE 1024

void usage(char *argv[]) {
    fprintf(stderr, "Usage: %s [-o outputfile] [-a appendfile] [-n showLineNumbers] file1 [file2 ...]\n", argv[0]);
    exit(EXIT_SUCCESS);
}

void display_file(const char *filename, int show_line_numbers) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    char buffer[BUFFER_SIZE];
    char line[BUFFER_SIZE];
    int line_number = 1;
    ssize_t bytes_read;
    int line_index = 0;

    while ((bytes_read = read(fd, buffer, sizeof(buffer))) > 0) {
        for (int i = 0; i < bytes_read; i++) {
            // Read one char at a time from the buffer to create a line
            line[line_index++] = buffer[i];

            // If we encounter a new line character, then the line is complete
            if (buffer[i] == '\n') {
                line[line_index] = '\0'; // Null-terminate the line

                // printf("%s", line);

                // Print the line number if enabled
                if (show_line_numbers) {
                    char line_number_str[32];
                    snprintf(line_number_str, sizeof(line_number_str), "%6d ", line_number++);
                    // printf("%s", line_number_str);
                    write(STDOUT_FILENO, line_number_str, strlen(line_number_str));
                }

                // Write the entire line
                write(STDOUT_FILENO, line, line_index);

                // Reset line index for the next line
                line_index = 0;
            }
        }
    }

    if (bytes_read == -1) {
        perror("Error reading file");
    }

    close(fd);
}

int main(int argc, char *argv[]) {

    if (argc < 2) {
        usage(argv);
    }

    int opt;
    char *output_file = NULL;
    int append_mode = 0;
    int show_line_numbers = 0;
    int output_fd = STDOUT_FILENO;

    // Parse command-line options using getopts
    while ((opt = getopt(argc, argv, "o:a:n")) != -1) {
        switch (opt) {
            case 'o':
                output_file = optarg;
                append_mode = 0;
                break;
            case 'a':
                output_file = optarg;
                append_mode = 1;
                break;
            case 'n':
                show_line_numbers = 1;
                break;
            default:
                usage(argv);
        }
    }

    // Debugging output to check what output_file is set
    // if (output_file) {
    //     printf("Output file set to: %s\n", output_file);
    // }

    // Set up output redirection if an output file is specified
    if (output_file) {
        int flags = O_WRONLY | O_CREAT;
        flags |= (append_mode ? O_APPEND : O_TRUNC);

        output_fd = open(output_file, flags, 0644);
        if (output_fd == -1) {
            perror("Error opening output file");
            fprintf(stderr, "Output file: %s\n", output_file);
            exit(EXIT_FAILURE);
        }

        // Redirect stdout to the output file
        if (dup2(output_fd, STDOUT_FILENO) == -1) {
            perror("Error duplicating file descriptor");
            close(output_fd);
            exit(EXIT_FAILURE);
        }

        close(output_fd);
    }


    for (int i = optind; i < argc; ++i) {
        display_file(argv[i], show_line_numbers);
    }

    return 0;
}
