#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

void usage(char *argv[]) {
    fprintf(stderr, "Usage: %s [-o outputfile] [-a appendfile] file1 [file2 ...]\n", argv[0]);
    exit(EXIT_FAILURE);
}

void display_file(const char *filename) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;

    while ((bytes_read = read(fd, buffer, sizeof(buffer))) > 0) {
        if (write(STDOUT_FILENO, buffer, bytes_read) != bytes_read) {
            perror("Error writing to stdout");
            close(fd);
            exit(EXIT_FAILURE);
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
    int output_fd = STDOUT_FILENO;

    // Parse command-line options using getopts
    while ((opt = getopt(argc, argv, "o:a:")) != -1) {
        switch (opt) {
            case 'o':
                output_file = optarg;
                append_mode = 0;
                break;
            case 'a':
                output_file = optarg;
                append_mode = 1;
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
        display_file(argv[i]);
    }

    return 0;
}
