#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s PATH\n", argv[0]);
        return 1;
    }
    char *input = argv[1];
    int num_directories = 1; 

    for (char *p = input; *p; p++) {
        if (*p == ':') num_directories++;
    }
    printf("num_directories: %d\n", num_directories);

    // char **directories = malloc(num_directories * sizeof(char*));
    // char *start = input;
    // int dir_index = 0;
    // for (char *p = input; ; p++) {
    //     if (*p == ':' || *p == '\0') {
    //         int length = p - start;
    //         directories[dir_index] = malloc(length + 1); 
    //         strncpy(directories[dir_index], start, length);
    //         directories[dir_index][length] = '\0'; 
    //         dir_index++;
    //         start = p + 1;
    //     }
    //     if (*p == '\0'){
    //         break;
    //     }
    // }

    // printf("simple-shell$: ");
    // for (char *directory = directories[0]; directory; directory++) {
    //     printf("%s\n", directory);
    // }
}

