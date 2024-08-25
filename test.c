#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    // Define the path to the executable
    char *path = "/bin/ls";

    // Define the arguments for the executable
    // The first argument should be the name of the executable itself
    char *args[] = { "ls", "-l", "/usr", NULL };

    // Execute the program
    if (execv(path, args) == -1) {
        perror("execv");
        exit(EXIT_FAILURE);
    }

    // This line will not be reached if execv is successful
    printf("This will not be printed if execv is successful\n");

    return 0;
}