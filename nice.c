#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/resource.h>
#include <errno.h>

void print_usage(char *prog_name) {
    fprintf(stderr, "Usage: %s <priority> <command> [args...]\n", prog_name);
    fprintf(stderr, "   <priority> : Set the nice value (range: -20 to 19)\n");
    fprintf(stderr, "                Negative values increase priority, positive values decrease priority.\n");
    fprintf(stderr, "   <command>  : Command to run with the specified nice value\n");
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        print_usage(argv[0]);
        return 1;
    }

    // Parse priority (nice value)
    int priority = atoi(argv[1]);
    if (priority < -20 || priority > 19) {
        fprintf(stderr, "Error: Priority value out of range (-20 to 19)\n");
        return 1;
    }

    // Set the priority of the current process
    if (setpriority(PRIO_PROCESS, 0, priority) != 0) {
        perror("Failed to set priority");
        return 1;
    }

    // Inform user about the set priority
    if (priority < 0) {
        printf("Running command with nice value: %d (Higher priority)\n", priority);
    } else {
        printf("Running command with nice value: %d (Lower priority)\n", priority);
    }

    // Execute the command (replace current process with new one)
    execvp(argv[2], &argv[2]);

    // If execvp fails, print an error
    perror("Failed to execute command");
    return 1;
}
