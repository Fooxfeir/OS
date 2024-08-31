/**
 * @file    simpleshell.c
 * @brief   Implementation of a simple command-line interpreter.
 *
 * This program functions as a simple shell that interprets and executes commands
 * provided by the user. It supports basic command execution by searching for
 * the command in specified directories, handling command parameters, and 
 * running commands in the foreground or background.
 *
 * Task 1:
 * - The shell accepts a set of directories as a parameter during initialization.
 * - It waits for a command input from the user, searches for the command in the 
 *   provided directories, and executes it using the execv system call.
 * - If the command is not found in any of the directories, an error message is displayed.
 * - The shell executes one command and then terminates.
 *
 * Task 2:
 * - The shell continues to wait for new commands after executing one, instead of terminating.
 * - The shell supports running commands in the background using the '&' symbol.
 * - The shell will terminate when the user inputs the 'exit' command.
 * - The shell handles empty input by simply displaying the prompt again.
 * - Fork system call is used to execute commands in child processes.
 *
 * @author  Guilhermo Mocelim - 223325
 * @author  Pedro Bernardo Calou - 186711
 * 
 * @date    August 30, 2024
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TRUE 1
#define FALSE 0


/**
 * @brief   Splits a string into an array of substrings based on a delimiter character.
 *
 * This function splits the input string into substrings using the specified delimiter.
 * It returns an array of strings, with each element being a substring of the original string.
 * The function also updates the integer pointed to by `num_substrings` with the number of substrings.
 *
 * @param[in]  input_str       The input string to be split.
 * @param[in]  delimiter       The delimiter character to split the string by.
 * @param[out] num_substrings  Pointer to an integer where the number of substrings will be stored.
 * @return     A dynamically allocated array of strings (substrings). Each string in the array is
 *             dynamically allocated. The caller is responsible for freeing the memory.
 */
char **split_string(const char *input_str, char delimiter, int *num_substrings) {
    char **substring_array = NULL;
    int substring_count = 0;
    const char *temp_ptr = input_str;
    char *token_ptr;
    char *input_copy = strdup(input_str); // Duplicate the input string to avoid modifying the original

    // Count the number of delimiters to determine the number of substrings
    while (*temp_ptr) {
        if (*temp_ptr == delimiter) {
            substring_count++;
        }
        temp_ptr++;
    }

    // Account for the last substring after the final delimiter (or if there's no delimiter)
    substring_count++;
    *num_substrings = substring_count;

    // Allocate memory for the array of strings
    substring_array = malloc(substring_count * sizeof(char *));
    if (substring_array == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    // Split the string using the delimiter
    int index = 0;
    token_ptr = strtok(input_copy, &delimiter);
    while (token_ptr != NULL) {
        substring_array[index] = strdup(token_ptr); // Duplicate the token to store it in the array
        if (substring_array[index] == NULL) {
            perror("strdup");
            exit(EXIT_FAILURE);
        }
        index++;
        token_ptr = strtok(NULL, &delimiter);
    }

    free(input_copy); // Free the copy of the input string

    return substring_array;
}


/**
 * @brief   Checks if a given path exists.
 *
 * This function checks whether a file or directory exists at the specified path.
 *
 * @param[in] path  The path to the file or directory.
 * @return  1 if the path exists, 0 if it does not exist.
 */
int path_exists(const char *path) {
    if (access(path, F_OK) == 0) {
        return 1; // Path exists
    } else {
        return 0; // Path does not exist
    }
}


/**
 * @brief   Checks if each path in a list of paths exists.
 *
 * This function iterates through an array of paths and checks whether each path exists
 * using the `path_exists` function. If any path does not exist, the function returns an error code.
 *
 * @param[in]  paths       Array of strings, where each string is a path to check.
 * @param[in]  num_paths   The number of paths in the array.
 * @return     0 if all paths exist, 1 if any path does not exist.
 */
int check_paths_exist(const char **paths, int num_paths) {
    for (int i = 0; i < num_paths; i++) {
        if (!path_exists(paths[i])) {
            fprintf(stderr, "Error: Path does not exist: %s\n", paths[i]);
            return 1; // Return error if any path does not exist
        }
    }
    return 0; // Return success if all paths exist
}


/**
 * @brief   Reads a line of input from the user and splits it into command arguments.
 *
 * This function reads a line of input from stdin, trims the newline character,
 * and then splits the input string into an array of substrings using the `split_string`
 * function with a space (' ') as the delimiter.
 *
 * @param[out] arg_count  Pointer to an integer where the number of arguments will be stored.
 * @return     A dynamically allocated array of strings (arguments). Each string in the array is
 *             dynamically allocated. The caller is responsible for freeing the memory.
 */
char **read_command_line_input(int *arg_count) {
    char *input_buffer = NULL;
    size_t buffer_length = 0;
    ssize_t input_length;

    // Read the line of input
    input_length = getline(&input_buffer, &buffer_length, stdin);

    if (input_length == -1) {
        perror("getline");
        exit(EXIT_FAILURE);
    }

    // Remove the newline character if present
    if (input_length > 0 && input_buffer[input_length - 1] == '\n') {
        input_buffer[input_length - 1] = '\0';
    }

    // Split the input line into arguments
    char **arguments = split_string(input_buffer, ' ', arg_count);

    free(input_buffer); // Free the input buffer

    return arguments;
}


/**
 * @brief Concatenates two strings with a chosen character between them.
 *
 * This function allocates memory for the concatenated result, including the separator
 * character and the null terminator. The caller is responsible for freeing the memory
 * allocated for the resulting string.
 *
 * @param first_str   The first string to concatenate.
 * @param second_str  The second string to concatenate.
 * @param separator   The character to insert between the two strings.
 * @return            A dynamically allocated string containing first_str, separator, and second_str.
 *                    Returns NULL if memory allocation fails.
 */
char *strcat_with_separator(const char *first_str, const char *second_str, char separator) {
    // Calculate the required size for the new string
    size_t first_len = strlen(first_str);
    size_t second_len = strlen(second_str);
    size_t result_len = first_len + second_len + 2; // +2 for separator and null terminator

    // Allocate memory for the result
    char *result_str = malloc(result_len);
    if (result_str == NULL) {
        perror("malloc");
        return NULL;
    }

    // Build the result string
    strcpy(result_str, first_str);                // Copy the first string
    result_str[first_len] = separator;            // Add the separator character
    strcpy(result_str + first_len + 1, second_str); // Copy the second string

    return result_str;
}


/**
 * @brief Executes a command by searching for it in the specified directories.
 *
 * This function iterates over a list of directories, constructs the full path for
 * the command, checks if the command exists, and executes it using execv.
 *
 * @param directories  An array of directory paths to search for the command.
 * @param num_directories The number of directories in the array.
 * @param args         An array of argument strings to pass to the command.
 * @return             1 if the command was found and executed, 0 otherwise.
 */
int execute_command(char **directories, int num_directories, char **args) {
    int command_found = 0;
    
    for (int i = 0; i < num_directories; i++) {
        // Construct the full path for the command
        char *command_path = strcat_with_separator(directories[i], args[0], '/');
        if (command_path == NULL) {
            fprintf(stderr, "Memory allocation failed for command path.\n");
            return 0;
        }

        // Check if the command exists
        if (path_exists(command_path)) {
            // Execute the command
            execv(command_path, args);

            // Handle the case where execv fails
            perror("execv failed");
            free(command_path);
            return 0;
        }

        // Free allocated memory if command not found
        free(command_path);
    }

    return command_found;
}


int main(int argc, char *argv[]) {
    char *paths = argv[1];
    int num_dirs;
    char **dirs = split_string(paths, ':', &num_dirs);

    if (check_paths_exist(dirs, num_dirs)) {
        return 1;
    }

    int arg_count;
    char **arg_values;

    while (TRUE) {
        printf("simple-shell$: ");
        arg_values = read_command_line_input(&arg_count);

        if (arg_count == 0) { // ENTER
            continue;
        }
        else if (!strcmp(arg_values[0], "exit")) {
            exit(0);
        }

        if (fork() != 0) { 
            if(strcmp(arg_values[arg_count - 1], "&") != 0){
                int status;
                waitpid(-1, &status, 0);
            }
        }
        else {
            if (!strcmp(arg_values[arg_count - 1], "&")) {
                arg_values[arg_count - 1] = NULL;
            }
            if (execute_command(dirs, num_dirs, arg_values)) {
                printf("Comando não encontrado.\n");
            }
        }
    }
}