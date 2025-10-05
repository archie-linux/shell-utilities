#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>

// Define constants for search modes
#define CASE_SENSITIVE 0
#define CASE_INSENSITIVE 1

// Define constants for file types
#define TYPE_ALL 0
#define TYPE_FILE 1
#define TYPE_DIR 2

// Function to convert size string (like 1K, 100M) to bytes
off_t parse_size(const char *size_str) {
    char unit = size_str[strlen(size_str) - 1];
    off_t size = strtoll(size_str, NULL, 10);

    switch(unit) {
        case 'K':
        case 'k':
            size *= 1024;
            break;
        case 'M':
        case 'm':
            size *= 1024 * 1024;
            break;
        case 'G':
        case 'g':
            size *= 1024 * 1024 * 1024;
            break;
        default: break; // If no unit, treat as bytes
    }

    return size;
}

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

int check_size(off_t file_size, const char *size_condition) {
    if (!size_condition) {
        return 1; // No size condition provided, match by default
    }

    char operator = size_condition[0];
    off_t size_limit = parse_size(size_condition + 1); // Parse size after the +/- operator

    if (operator == '+') {
        return file_size > size_limit;
    } else if (operator == '-') {
        return file_size < size_limit;
    }
    return 1;
}

// The function checks if the bitwise AND of file_mode and permissions equals permissions.
// This verifies whether all specified permissions are set on the file.
int check_permissions(mode_t file_mode, const char *perm_condition) {
    // Parse permissions
    mode_t permissions = 0;

    // Check each character in the permission string
    if (perm_condition) {
        for (size_t i = 0; i < strlen(perm_condition); i++) {
            char perm = perm_condition[i];
            if (perm == 'r') {
                permissions |= S_IRUSR;
            } else if (perm == 'w') {
                permissions |= S_IWUSR;
            } else if (perm == 'x') {
                permissions |= S_IXUSR;
            }
            // Add more conditions for group and others if needed
        }
    }

    // Compare with file's permissions
    return (file_mode & permissions) == permissions;
}

// The function determines if the file is owned by a specific user
int check_user(uid_t file_uid, const char *user) {
    if (!user) return 1;

    struct passwd *pw = getpwnam(user);
    if (!pw) {
        fprintf(stderr, "User not found: %s\n", user);
        return 0;
    }
    return file_uid == pw->pw_uid;
}

// The function determines if the file is owned by a specific group
int check_group(gid_t file_gid, const char *group) {
    if (!group) return 1;

    struct group *gr = getgrnam(group);
    if (!gr) {
        fprintf(stderr, "Group not found: %s\n", group);
        return 0;
    }
    return file_gid == gr->gr_gid;
}

void list_files(const char *path,
                const char *name_pattern,
                int search_mode,
                int type_filter,
                const char *size_condition,
                const char *perm_condition,
                const char *user,
                const char *group) {

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
        int size_matches = check_size(statbuf.st_size, size_condition);
        int perm_matches = check_permissions(statbuf.st_mode, perm_condition);
        int user_matches = check_user(statbuf.st_uid, user);
        int group_matches = check_group(statbuf.st_gid, group);

        // Print directories if they match the criteria
        if (type_matches && S_ISDIR(statbuf.st_mode) && name_matches && size_matches
                && perm_matches && user_matches && group_matches) {
            printf("%s\n", full_path);  // Print the directory path
        }

        // Always recurse into directories
        if (S_ISDIR(statbuf.st_mode)) {
            // Recurse into the directory to list its contents
            list_files(full_path,
                       name_pattern,
                       search_mode,
                       type_filter,
                       size_condition,
                       perm_condition,
                       user,
                       group);
        }

        // Print the entry if it matches the name and type
        if (name_matches && type_matches && size_matches && S_ISREG(statbuf.st_mode)
                && perm_matches && user_matches && group_matches) {
            printf("%s\n", full_path);  // Print the file path
        }
    }

    closedir(dp);
}

void print_usage() {
    printf("Usage: find [path] [options]\n");
    printf("Options:\n");
    printf("  -name <filename>        Search for files by name (case-sensitive)\n");
    printf("  -iname <filename>       Search for files by name (case-insensitive)\n");
    printf("  -type <type>           Specify the type of file to search for:\n");
    printf("                          'f' for regular files, 'd' for directories\n");
    printf("  -s <size>              Filter files by size. Use:\n");
    printf("                          -1K for smaller than 1 Kilobyte,\n");
    printf("                          +100M for larger than 100 Megabytes,\n");
    fprintf(stderr, "  -perm <mode>      Search for files with specific permissions.\n");
    fprintf(stderr, "  -user <username>  Search for files owned by a specific user.\n");
    fprintf(stderr, "  -group <groupname> Search for files owned by a specific group.\n");
    printf("  -h, --help             Display this help message and exit\n");
}

int main(int argc, char *argv[]) {
    const char *path = NULL;
    const char *name_pattern = NULL;
    int search_mode = CASE_SENSITIVE;  // Default is case-sensitive search
    int type_filter = TYPE_ALL;
    const char *size_condition = NULL;
    const char *perm_condition = NULL;
    const char *user = NULL;
    const char *group = NULL;

    // Parse command-line arguments
    if (argc < 2 || strcmp(argv[1], "-h") == 0) {
        print_usage();
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
        } else if (strcmp(argv[i], "-size") == 0 && i + 1 < argc) {
            size_condition = argv[i+1];
            i++;
        } else if (strcmp(argv[i], "-perm") == 0 && i + 1 < argc) {
            perm_condition = argv[i+1];
            i++;
        } else if (strcmp(argv[i], "-user") == 0 && i + 1 < argc) {
            user = argv[i+1];
            i++;
        } else if (strcmp(argv[i], "-group") == 0 && i + 1 < argc) {
            group = argv[i+1];
            i++;
        }
    }

    // printf("Flags: -name: %s -type: %d\n", name_pattern, type_filter);
    // fflush(stdout);

    printf("Starting directory traversal for path: %s\n", path);  // Debug for initial path
    fflush(stdout);

    // Call the list_files function with the name pattern
    list_files(path, name_pattern, search_mode, type_filter,
                size_condition, perm_condition, user, group);

    printf("Finished directory traversal.\n");  // Debug when done
    fflush(stdout);

    return EXIT_SUCCESS;
}
