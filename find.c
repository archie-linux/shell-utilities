#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

void list_files(const char *path) {
    struct dirent *entry;
    struct stat statbuf;
    DIR *dp = opendir(path);

    if (dp == NULL) {
        perror("opendir");
        return;
    }

    printf("Opening directory: %s\n", path);  // Debug print for current directory
    fflush(stdout);

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

        // If it's a directory, call list_files recursively
        if (S_ISDIR(statbuf.st_mode)) {
            printf("%s (directory)\n", full_path);
            fflush(stdout);
            list_files(full_path);
        } else {
            printf("%s (file)\n", full_path);
            fflush(stdout);
        }
    }

    closedir(dp);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <path>\n", argv[0]);
        fflush(stdout);
        return EXIT_FAILURE;
    }

    list_files(argv[1]);
    return EXIT_SUCCESS;
}
