#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

// Define constants for search modes
#define CASE_SENSITIVE 0
#define CASE_INSENSITIVE 1

// Define constants for file types
#define TYPE_ALL 0
#define TYPE_FILE 1
#define TYPE_DIR 2

int check_name(const char *entry_name, const char *name_pattern, int search_mode) {
        // Check if the name matches the provided pattern
        int name_matches = (name_pattern == NULL) ||
                           (search_mode == CASE_SENSITIVE && strcmp(entry_name, name_pattern) == 0) ||
                           (search_mode == CASE_INSENSITIVE && strcasecmp(entry_name, name_pattern) == 0);

        return name_matches;
}

int check_type(struct stat *statbuf, int type_filter) {
        // Determine if the entry is a file or directory
        int is_directory = S_ISDIR(statbuf->st_mode);
        int is_regular_file = S_ISREG(statbuf->st_mode);

        // Check for type matches
        int type_matches = (type_filter == TYPE_ALL) ||
                           (type_filter == TYPE_FILE && is_regular_file) ||
                           (type_filter == TYPE_DIR && is_directory);

        return type_matches;
}

void list_files(const char *path, char *name_pattern, int search_mode, int type_filter) {
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

        int name_matches = check_name(entry->d_name, name_pattern, search_mode);
        int type_matches = check_type(&statbuf, type_filter);

        // Print directories if they match the criteria
        if (type_matches && S_ISDIR(statbuf.st_mode) && name_matches) {
            printf("%s\n", full_path);  // Print the directory path
        }

        // Always recurse into directories
        if (S_ISDIR(statbuf.st_mode)) {
            // Recurse into the directory to list its contents
            list_files(full_path, name_pattern, search_mode, type_filter);
        }

        // Print the entry if it matches the name and type
        if (name_matches && type_matches && S_ISREG(statbuf.st_mode)) {
            printf("%s\n", full_path);  // Print the file path
        }
    }

    closedir(dp);
}

int main(int argc, char *argv[]) {
    char *path = NULL;
    char *name_pattern = NULL;
    int search_mode = CASE_SENSITIVE;  // Default is case-sensitive search
    int type_filter = TYPE_ALL;

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
            i++;  // Move past the filename
        } else if (strcmp(argv[i], "-iname") == 0 && i + 1 < argc) {
            name_pattern = argv[i + 1];  // Store the filename pattern
            search_mode = CASE_INSENSITIVE;  // Case-insensitive search
            i++;  // Move past the filename
        } else if (strcmp(argv[i], "-type") == 0 && i + 1 < argc) {
            if (strcmp(argv[i + 1], "f") == 0) {
                type_filter = TYPE_FILE;  // Filter for regular files
            } else if (strcmp(argv[i + 1], "d") == 0) {
                type_filter = TYPE_DIR;  // Filter for directories
            } else {
                fprintf(stderr, "Unknown type: %s. Use 'f' or 'd'.\n", argv[i + 1]);
                return EXIT_FAILURE;
            }
            i++;  // Move past the type
        }
    }

    // printf("Flags: -name: %s -type: %d\n", name_pattern, type_filter);
    // fflush(stdout);

    printf("Starting directory traversal for path: %s\n", path);  // Debug for initial path
    fflush(stdout);

    // Call the list_files function with the name pattern
    list_files(path, name_pattern, search_mode, type_filter);

    printf("Finished directory traversal.\n");  // Debug when done
    fflush(stdout);

    return EXIT_SUCCESS;
}
