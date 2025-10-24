#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include "patterns.h"

int main(int argc, char *argv[]) {
    // Check for user args
    if (argc == 1){
        print_errs(ERR_SERV);
    }

    // Parse user args using getopt
    int numargs = arg_parse_server(argc, argv);

    // Response vars
    char ack[3] = {'A', 'C', 'K'};
    char nack[4] = {'N','A', 'C', 'K'};

    // Creating socket
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    // Set socket settings
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt,sizeof(opt));
    // Bind socket to address
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(portnum);
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0){
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    // Listen for client
    if (listen(server_fd, 3) < 0){
        perror("listen failed");
        exit(EXIT_FAILURE);
    }
    // Accept connection
    int new_socket = accept(server_fd, (struct sockaddr*)&address, &addrlen);
    if (new_socket < 0){
        perror("accept failed");
        exit(EXIT_FAILURE);
    }

    // Listens for recieving file
    while(1){
        // Stuff here

    }

    // closing the connected socket
    close(new_socket);
  
    // closing the listening socket
    close(server_fd);
    return 0;
}