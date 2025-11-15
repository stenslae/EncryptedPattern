#include "patterns.h"

/**
 * arg_parse() - Set user input flags
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
int arg_parse_client(int argc, char **argv, char **filename, uint32_t **led_pattern, int **delay_pattern){
    int numargs = 0;

    int opt;
    while ((opt = getopt(argc, argv, ":p:f:hv")) != -1){
        switch (opt){
        case 'p':
            p = true;
            char *temp = strdup(optarg);
            
            char *led_pattern_str = strtok(temp, ",");
            char *delay_pattern_str = strtok(NULL, ",");
            if (!led_pattern_str || !delay_pattern_str){
                free(*delay_pattern);
                free(*led_pattern);
                print_errs(ERR_INVALID_FORMAT);
            }

            *led_pattern = realloc(*led_pattern, (numargs + 1) * sizeof(uint32_t));
            *delay_pattern = realloc(*delay_pattern, (numargs + 1) * sizeof(int));

            (*led_pattern)[numargs] = strtol(led_pattern_str, NULL, 16);
            (*delay_pattern)[numargs] = atoi(delay_pattern_str);

            if ((*led_pattern)[numargs] == 0 || (*delay_pattern)[numargs] == 0){
                free(*delay_pattern);
                free(*led_pattern);
                print_errs(ERR_INVALID_FORMAT);
            }

            numargs++;
            break;
        case 'f':
            f = true;
            *filename = strdup(optarg);
            break;
        case 't':
            portnum = strtoi(optarg);
            break;
        case 'v':
            v = true;
            break;
        case 'h':
            h = true;
            break;
        case ':':
            // Missing value after option
            fprintf(stderr, "Option -%c needs a value.\n", optopt);
            break;
        case '?':
            // Unknown option
            fprintf(stderr, "Unknown option: -%c\n", optopt);
            break;
        }
    }

    // Parse the file
    if (f){
        numargs = parse_file(*filename, led_pattern, delay_pattern);
    }

    // Error handle help, no args, or incompatible hards
    if (!(p || f || h || v) || (p && f) || h || numargs == 0){
        free(*delay_pattern);
        free(*led_pattern);
        print_errs(ERR_CONFLICTING_ARGS);
    }

    return numargs;
}

/**
 * arg_parse_server() - Set user input flags
 * @argc: Number of user args
 * @**argv: User args
 *
 * Uses getopt() to read the port flag the user set.
 *
 * Return: 0
 */
int arg_parse_server(int argc, char **argv){
    int opt;
    while ((opt = getopt(argc, argv, ":p:f:hv")) != -1){
        switch (opt){
        case 't':
            portnum = strtoi(optarg);
            break;
        case ':':
            // Missing value after option
            fprintf(stderr, "Option -%c needs a value.\n", optopt);
            break;
        case '?':
            // Unknown option
            fprintf(stderr, "Unknown option: -%c\n", optopt);
            break;
        }
    }

    if(portnum == 0 || portnum == NULL){
        print_errs(ERR_SERV);
    }
}

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
int parse_file(char *filename, uint32_t **led_pattern, int **delay_pattern){
    int numargs = 0;
    FILE *fptr = fopen(filename, "r");
    if (fptr == NULL){
        free(*delay_pattern);
        free(*led_pattern);
        fclose(fptr);
        print_errs(ERR_FILE_NOT_FOUND);
    }

    // Read through file and collect patterns
    char temp[LINE_BUF_SIZE];
    while (fgets(temp, sizeof(temp), fptr) != NULL){
        char *led_pattern_str = strtok(temp, " ");
        char *delay_pattern_str = strtok(NULL, " ");
        if (!led_pattern_str || !delay_pattern_str) {
            free(*delay_pattern);
            free(*led_pattern);
            fclose(fptr);
            print_errs(ERR_INVALID_FORMAT_FILE);
        }

        *led_pattern = realloc(*led_pattern, (numargs + 1) * sizeof(uint32_t));
        *delay_pattern = realloc(*delay_pattern, (numargs + 1) * sizeof(int));

        (*led_pattern)[numargs] = strtol(led_pattern_str, NULL, 16);
        (*delay_pattern)[numargs] = atoi(delay_pattern_str);
        
        if ((*led_pattern)[numargs] == 0 || (*delay_pattern)[numargs] == 0)
        {
            free(*delay_pattern);
            free(*led_pattern);
            fclose(fptr);
            print_errs(ERR_INVALID_FORMAT);
        }
        numargs ++;
    }

    fclose(fptr);
    return numargs;
}

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
void print_errs(int errtype){
    if(errtype == ERR_SERV){
        printf(
            "Usage:\n"
            "  ./server -t PORT \n\n"
            "Description:\n"
            "  Recieve an encrypted 4 byte message pattern to a server.\n\n"
            "Arguments:\n"
            "  -t PORT               set the port for TCP.\n"
        );
        exit(0);
    }
    
    if (p && f){
        printf("Error 1: Both -p and -f are given as arguments. These "
               "arguments are mutually exclusive.\n");
    }
    else if (errtype == ERR_FILE_NOT_FOUND){
        printf("Error 2: The filename given to after the -f option doesn’t "
               "exist.\n");
    }
    else if (errtype == ERR_INVALID_FORMAT){
        printf("Error 3: The format of the argument after the -p flag is "
               "incorrect.\n");
    }
    else if (errtype == ERR_INVALID_FORMAT_FILE){
        printf("Error 4: The format of the file for -f is "
               "incorrect.\n");
    }else if (errtype == ERR_BAD_ENCRYPT){
        printf("ERROR: Encryption/decryption process failed. Please try again.\n");
    }

    printf(
        "Usage:\n"
        "  ./client [-h] [-v] ([-f FILE] | [-p VALUE,TIME])\n\n"
        "Description:\n"
        "  Send an encrypted 4 byte message pattern to a server.\n\n"
        "Arguments:\n"
        "  -h                    show this help message and exit\n"
        "  -v                    enable verbose output\n"
        "  -p VALUE,TIME         specify pattern with\n"
        "                        VALUE (hex) and TIME (ms).\n"
        "                        can be used multiple times to create a sequence.\n"
        "  -f FILE               specify a text file with\n"
        "                        <VALUE> <TIME> lines\n\n"
        "Example:\n"
        "  ./client -t 4090 -p 0x55,500 -p 0x0f,1500\n"
    );

    exit(0);
}