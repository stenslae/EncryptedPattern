#include <fcntl.h>  // for file open flags
#include <signal.h> // for ctrl-c
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h> // for mmap
#include <unistd.h>   // for getting the page size and getopt

#define ERR_CONFLICTING_ARGS 1
#define ERR_FILE_NOT_FOUND 2
#define ERR_INVALID_FORMAT 3
#define ERR_INVALID_FORMAT_FILE 4
#define LINE_BUF_SIZE 128

// Arg Flags
bool p = false;
bool f = false;
bool h = false;
bool v = false;

// Runtime Function
static volatile sig_atomic_t keepRunning = 1;
void intHandler(int dummy)
{
    keepRunning = 0;
}

// Function Declarations
void print_errs(int errtype);
int arg_parse(int argc, char **argv, char **filename, uint32_t **led_pattern, int **delay_pattern);
int write_mem(uint32_t address, uint32_t value, size_t PAGE_SIZE, int fd);
int parse_file(char *filename, uint32_t **led_pattern, int **delay_pattern);

int main(int argc, char **argv)
{
    // Addresses on SoC
    const uint32_t HPS_LED_CONTROL = 0xff200000;
    const uint32_t LED_REG = 0xff200004;
    
    // Size of a page of memory in the system. Typically 4096 bytes.
    const size_t PAGE_SIZE = sysconf(_SC_PAGE_SIZE);

    // Check for user args
    if (argc == 1)
    {
        print_errs(ERR_INVALID_FORMAT);
    }

    // Parse user args using getopt
    char *filename = NULL;
    uint32_t *led_pattern = NULL;
    int *delay_pattern = NULL;
    int numargs = arg_parse(argc, argv, &filename, &led_pattern, &delay_pattern);

    // Open the /dev/mem file, which is an image of the main system memory.
    int fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd == -1)
    {
        fprintf(stderr, "failed to open /dev/mem.\n");
        exit(1);
    }

    // Turn on SW Control
    int err = write_mem(HPS_LED_CONTROL, 1, PAGE_SIZE, fd);
    if (err == 1)
    {
        fprintf(stderr, "failed to write to memory.\n");
        exit(1);
    }

    // Keep track of which pattern to display
    int count = 0;

    // Run patterns until CTRL^C runs
    signal(SIGINT, intHandler);
    while (keepRunning)
    {
        // Iterate pattern out
        err = write_mem(LED_REG, led_pattern[count], PAGE_SIZE, fd);
        if (err == 1)
        {
            fprintf(stderr, "failed to write to memory.\n");
            exit(1);
        }

        // Verbose Output
        if (v)
        {
            printf("LED Pattern: 0x%x Display Time: %d ms\n",
                   led_pattern[count],
                   delay_pattern[count]);
        }

        // Time is in ms
        usleep(delay_pattern[count] * 1000);

        // Iterate to next pattern
        if (count >= numargs-1)
        {
            if (f)
            {
                // If it is a -f it will terminate
                break;
            }
            else
            {
                // If it is a -p, will repeat
                count = 0;
            }
        }
        else
        {
            count++;
        }
    }

    // Turn off SW Control
    err = write_mem(HPS_LED_CONTROL, 0, PAGE_SIZE, fd);
    if (err == 1)
    {
        fprintf(stderr, "failed to write to memory.\n");
        exit(1);
    }

    printf("\n");
    free(led_pattern);
    free(delay_pattern);
    return 0;
}

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
int arg_parse(int argc, char **argv, char **filename, uint32_t **led_pattern, int **delay_pattern)
{
    int numargs = 0;

    int opt;
    while ((opt = getopt(argc, argv, ":p:f:hv")) != -1)
    {
        switch (opt)
        {
        case 'p':
            p = true;
            char *temp = strdup(optarg);
            
            char *led_pattern_str = strtok(temp, ",");
            char *delay_pattern_str = strtok(NULL, ",");
            if (!led_pattern_str || !delay_pattern_str) {
                free(*delay_pattern);
                free(*led_pattern);
                print_errs(ERR_INVALID_FORMAT);
            }

            *led_pattern = realloc(*led_pattern, (numargs + 1) * sizeof(uint32_t));
            *delay_pattern = realloc(*delay_pattern, (numargs + 1) * sizeof(int));

            (*led_pattern)[numargs] = strtol(led_pattern_str, NULL, 16);
            (*delay_pattern)[numargs] = atoi(delay_pattern_str);

            if ((*led_pattern)[numargs] == 0 || (*delay_pattern)[numargs] == 0)
            {
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
    if (f)
    {
        numargs = parse_file(*filename, led_pattern, delay_pattern);
    }

    // Error handle help, no args, or incompatible hards
    if (!(p || f || h || v) || (p && f) || h || numargs == 0)
    {
        free(*delay_pattern);
        free(*led_pattern);
        print_errs(ERR_CONFLICTING_ARGS);
    }

    return numargs;
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
int parse_file(char *filename, uint32_t **led_pattern, int **delay_pattern)
{
    int numargs = 0;
    FILE *fptr = fopen(filename, "r");
    if (fptr == NULL)
    {
        free(*delay_pattern);
        free(*led_pattern);
        fclose(fptr);
        print_errs(ERR_FILE_NOT_FOUND);
    }

    // Read through file and collect patterns
    char temp[LINE_BUF_SIZE];
    while (fgets(temp, sizeof(temp), fptr) != NULL)
    {
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
 * write_mem() - Brief description of function.
 * @address: Physical memory address we want to write to.
 * @value: Value to be written to memory.
 * @PAGE_SIZE: Size of memory page in system. Typically 4096 bytes. 
 * @fd: /dev/mem file.
 *
 * Pull one page of memory containing our address 
 * using mmap() and calculate the address's offset. 
 * Then the virtual memory page, accounting for 
 * offset gets a pointer to the address we want to 
 * write to. The value at the pointer is then 
 * written to value.
 *
 * Return: 1 for error, 0 for success
 */
int write_mem(uint32_t address, uint32_t value, size_t PAGE_SIZE, int fd)
{
    uint32_t page_aligned_addr = address & ~(PAGE_SIZE - 1);

    // Mmap takes a page of phsyical memory into virtual memory
    uint32_t *page_virtual_addr = (uint32_t *) mmap(NULL,PAGE_SIZE, PROT_READ 
        | PROT_WRITE, MAP_SHARED, fd,page_aligned_addr);
    if (page_virtual_addr == MAP_FAILED)
    {
        fprintf(stderr, "failed to map memory.\n");
        return 1;
    }

    uint32_t offset_in_page = address & (PAGE_SIZE - 1);

    volatile uint32_t *target_virtual_addr =
        page_virtual_addr + offset_in_page / sizeof(uint32_t *);

    *target_virtual_addr = value;

    return 0;
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
void print_errs(int errtype)
{
    if (p && f)
    {
        printf("Error 1: Both -p and -f are given as arguments. These "
               "arguments are mutually exclusive.\n");
    }
    else if (errtype == ERR_FILE_NOT_FOUND)
    {
        printf("Error 2: The filename given to after the -f option doesn’t "
               "exist.\n");
    }
    else if (errtype == ERR_INVALID_FORMAT)
    {
        printf("Error 3: The format of the argument after the -p flag is "
               "incorrect.\n");
    }
    else if (errtype == ERR_INVALID_FORMAT_FILE)
    {
        printf("Error 4: The format of the file for -f is "
               "incorrect.\n");
    }

    printf(
        "Usage:\n"
        "  ./led_patterns [-h] [-v] ([-f FILE] | [-p VALUE,TIME])\n\n"
        "Description:\n"
        "  Set LED patterns on the SoC device.\n\n"
        "Arguments:\n"
        "  -h                    show this help message and exit\n"
        "  -v                    enable verbose output\n"
        "  -p VALUE,TIME         specify LED pattern with\n"
        "                        VALUE (hex) and TIME (ms).\n"
        "                        can be used multiple times to create a sequence.\n"
        "  -f FILE               specify a text file with\n"
        "                        <VALUE> <TIME> lines\n\n"
        "Example:\n"
        "  ./led_patterns -p 0x55,500 -p 0x0f,1500\n"
    );

    exit(0);
}
