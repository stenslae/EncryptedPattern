#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include "patterns.h"

// Function breaks running with CTRL^C
static volatile sig_atomic_t keepRunning = 1;
void intHandler(int dummy){
    keepRunning = 0;
}

int main(int argc, char *argv[]){
    // Check for user args
    if (argc == 1){
        print_errs(ERR_INVALID_FORMAT);
    }

    // Parse user args using getopt
    char *filename = NULL;
    uint32_t *led_pattern = NULL;
    int *delay_pattern = NULL;
    int numargs = arg_parse_client(argc, argv, &filename, &led_pattern, &delay_pattern);

    // Response vars
    char ack[3] = {'A', 'C', 'K'};
    char nack[4] = {'N','A', 'C', 'K'};

    // Creating socket
    struct sockaddr_in serv_addr;
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    // Bind socket to port
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portnum);
    // Set client encoding settings
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);
    // Connect to address
    if (connect(client_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection failed");
        close(client_fd);
        exit(1);
    }

    // Keep track of which pattern to display
    int count = 0;

    // Run patterns until CTRL^C runs
    signal(SIGINT, intHandler);
    while (keepRunning){
        // send Iterate pattern out

        // Verbose Output
        if (v){
            printf("Stending Pattern: % Delay Time: %d ms\n",
                   led_pattern[count],
                   delay_pattern[count]);
        }

        // Time is in ms
        usleep(delay_pattern[count] * 1000);

        // -f will terminate, -p will repeat
        if (count >= numargs-1){
            if (f){
                break;
            }else{
                count = 0;
            }
        }else{
            count++;
        }
    }

    // TODO: Send close packet
    

    printf("\n");
    free(led_pattern);
    free(delay_pattern);
    close(client_fd);
    return 0;
}