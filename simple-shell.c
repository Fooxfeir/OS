#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

ssize_t my_getline(char **lineptr, size_t *n, FILE *stream);

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
    //test

    printf("simple-shell$: ");

    char *line;
    size_t len = 0;
    my_getline(&line, &len, stdin);
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
        printf("Trying to execute %s\n", path);
        execv(path, tokens);
    }
    printf("Command not found\n");

    free(line);
}

ssize_t my_getline(char **lineptr, size_t *n, FILE *stream) {
    if (lineptr == NULL || n == NULL || stream == NULL) {
        return -1;
    }

    char *buf = *lineptr;
    size_t size = *n;

    if (buf == NULL || size == 0) {
        size = 128; // Initial buffer size
        buf = malloc(size);
        if (buf == NULL) {
            return -1;
        }
    }

    int c;
    size_t len = 0;

    while ((c = fgetc(stream)) != EOF) {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (new_buf == NULL) {
                free(buf);
                return -1;
            }
            buf = new_buf;
        }
        buf[len++] = c;
        if (c == '\n') {
            break;
        }
    }

    if (len == 0) {
        free(buf);
        return -1;
    }

        buf[len] = '\0';
    *lineptr = buf;
    *n = size;

    return len;
}

