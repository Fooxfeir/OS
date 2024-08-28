//Guilhermo Mocelim - 223325
//Pedro Bernardo Calou - 186711

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

    char **directories = malloc(num_directories * sizeof(char*));
    char *start = input;
    int dir_index = 0;
    for (char *p = input; ; p++) {
        if (*p == ':' || *p == '\0') {
            int length = p - start;
            directories[dir_index] = malloc(length + 1); 
            strncpy(directories[dir_index], start, length);
            directories[dir_index][length] = '\0'; 
            dir_index++;
            start = p + 1;
        }
        if (*p == '\0'){
            break;
        }
    }

    printf("simple-shell$: ");

    char *line;
    size_t len = 0;
    getline(&line, &len, stdin);
    line[strcspn(line, "\n")] = 0;

    char *delimiter = " ";
    char *token;
    char **tokens = NULL;
    int num_tokens = 0;

    token = strtok(line, delimiter);
    while (token != NULL) {
        num_tokens++;
        tokens = realloc(tokens, (num_tokens) * sizeof(char*));
        tokens[num_tokens - 1] = strdup(token);

        token = strtok(NULL, delimiter);
    }
    num_tokens++;
    tokens = realloc(tokens, (num_tokens) * sizeof(char*));
    tokens[num_tokens - 1] = NULL;

    for (int i = 0; i < num_directories; i++) {
        char path[1024];
        snprintf(path, sizeof(path), "%s/%s", directories[i], tokens[0]);
        execv(path, tokens);
    }
    printf("Command not found\n");

    free(line);
    for (int i = 0; i < num_directories; i++) {
        free(directories[i]);
    }
    free(directories);
    free(tokens);
}