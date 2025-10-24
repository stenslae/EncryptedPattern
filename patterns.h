#include <signal.h> // for ctrl-c
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>   // for getopt

#define ERR_CONFLICTING_ARGS 1
#define ERR_FILE_NOT_FOUND 2
#define ERR_INVALID_FORMAT 3
#define ERR_INVALID_FORMAT_FILE 4
#define ERR_SERV 5
#define LINE_BUF_SIZE 128

// Arg Flags
bool p = false;
bool f = false;
bool h = false;
bool v = false;
int portnum = 0;

/**
 * arg_parse_client() - Set user input flags
 * @argc: Number of user args
 * @**argv: User args
 * @**filename: Points to *filename in main
 * @**led_pattern: Points to *led_pattern in main
 * @**delay_pattern: Points to *delay_pattern in main
 *
 * Uses getopt() to read each flag user has set. Saves arguments for
 * -p and -f. Also checks for any unexpected flags.
 *
 * If multiple -p flags are present, each arg is read and, tokenized
 * based on the "," delimiter, and realloc *led_pattern and 
 * *delay_pattern to add the typecasted tokenized chars to them.
 *
 * Return: Number of values in *led_pattern and *delay_pattern
 */
int arg_parse_client(int argc, char **argv, char **filename, uint32_t **led_pattern, int **delay_pattern);
/**
 * arg_parse_server() - Set user input flags
 * @argc: Number of user args
 * @**argv: User args
 *
 * Uses getopt() to read the port flag the user set.
 *
 * Return: 0
 */
int arg_parse_server(int argc, char **argv);
/*
 * parse_file() - Parse file into *led_pattern and *delay_pattern
 * @*filename: The name of the file being read.
 * @**led_pattern: Describe the second argument.
 * @**delay_pattern:
 *
 * Opens the file in read mode and handles if no file was found.
 * Then iterates through each line with read buffer, tokenizing
 * the char[] based on the " " delimiter in the file, and realloc
 * *led_pattern and *delay_pattern to add the typecasted tokenized
 * chars to them.
 *
 * Return: Number of values in *led_pattern and *delay_pattern
 */
int parse_file(char *filename, uint32_t **led_pattern, int **delay_pattern);
/**
 * print_errs() - Print error and help of function
 * @errtype: Flag for specific errors
 *
 * Called when help is flagged, no file is found, or
 * incompatible flags are sent by user. Will print
 * applicable error message and the help/usage prompt.
 *
 * Return: void
 */
void print_errs(int errtype);