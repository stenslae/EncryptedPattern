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
#define ERR_BAD_ENCRYPT 6
#define LINE_BUF_SIZE 128

// Arg Flags
bool p = false;
bool f = false;
bool h = false;
bool v = false;
int portnum = 0;

int arg_parse_client(int argc, char **argv, char **filename, uint32_t **led_pattern, int **delay_pattern);
int arg_parse_server(int argc, char **argv);
int parse_file(char *filename, uint32_t **led_pattern, int **delay_pattern);
void print_errs(int errtype);