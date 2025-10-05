#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

// Define constants for search modes
#define CASE_SENSITIVE 0
#define CASE_INSENSITIVE 1

void list_files(const char *path, char *name_pattern, int search_mode) {
    struct dirent *entry;
    struct stat statbuf;
    DIR *dp = opendir(path);

    if (dp == NULL) {
        perror("opendir");
        return;
    }

    // printf("Opening directory: %s\n", path);  // Debug print for current directory
    // fflush(stdout);

    while ((entry = readdir(dp)) != NULL) {
        // printf("Found entry: %s\n", entry->d_name);  // Debug print for each directory entry
        fflush(stdout);

        char full_path[1024];
        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);
        // printf("Full path: %s\n", full_path);  // Debug print for full path
        // fflush(stdout);

        // Skip the "." and ".." directories and hidden files
        if (entry->d_name[0] == '.')
            // printf("Skipping hidden file/directory: %s\n", entry->d_name);  // Debug for hidden files
            continue;

        // Check file stats
        if (stat(full_path, &statbuf) == -1) {
            perror("stat");
            continue;
        }

        // Check if the name matches the provided pattern (if specified)
        if (name_pattern != NULL) {
            int match = 0;
            if (search_mode == CASE_SENSITIVE) {
                // Case-sensitive search
                match = (strcmp(entry->d_name, name_pattern) == 0);
            } else if (search_mode == CASE_INSENSITIVE) {
                // Case-insensitive search
                match = (strcasecmp(entry->d_name, name_pattern) == 0);
            }

            if (!match) {
                // printf("Skipping non-matching file: %s\n", entry->d_name);  // Debug for non-matching file
                // fflush(stdout);
                continue;
            }
        }

        // If it's a directory, call list_files recursively
        if (S_ISDIR(statbuf.st_mode)) {
            printf("%s\n", full_path);
            fflush(stdout);
            list_files(full_path, name_pattern, search_mode);
        } else {
            printf("%s\n", full_path);
            fflush(stdout);
        }
    }

    closedir(dp);
}

int main(int argc, char *argv[]) {
    char *path = NULL;
    char *name_pattern = NULL;
    int search_mode = CASE_SENSITIVE;  // Default is case-sensitive search


    // Parse command-line arguments
    if (argc < 2) {
        fprintf(stderr, "Usage: %s path [-name <filename>] [-iname <filename>]\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Process the arguments
    path = argv[1]; // First argument is always the path

    // Check if the "-name" flag is provided
    for (int i=2; i < argc; i++) {
        if (strcmp(argv[i], "-name") == 0 && i + 1 < argc) {
            name_pattern = argv[i + 1]; // Store the filename pattern
            search_mode = CASE_SENSITIVE; // Case-sensitive search
            break;
        } else if (strcmp(argv[i], "-iname") == 0 && i + 1 < argc) {
            name_pattern = argv[i + 1];  // Store the filename pattern
            search_mode = CASE_INSENSITIVE;  // Case-insensitive search
            break;
        }
    }

    printf("Starting directory traversal for path: %s\n", path);  // Debug for initial path
    fflush(stdout);

    // Call the list_files function with the name pattern
    list_files(path, name_pattern, search_mode);

    printf("Finished directory traversal.\n");  // Debug when done
    fflush(stdout);

    return EXIT_SUCCESS;
}
