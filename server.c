// Server side C/C++ program to demonstrate Socket programming
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int port;
char *trader_pass[10] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10"};

int login(int new_socket)
{
    char buffer[1024] = {0}, ack[1024] = {0};
    read(new_socket, buffer, 1024);

    char *username = strtok(buffer, " ");
    char *password = strtok(NULL, " ");

    int result = 0;
    if (strcmp(trader_pass[atoi(username)-1], password))
        strcpy(ack, "No\0");
    else {
        result = atoi(username);
        strcpy(ack, "Yes\0");
    }
    printf("Trader %s logged in\n", username);
    send(new_socket, ack, strlen(ack), 0);
    return result;
}
int main(int argc, char const *argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *hello = "Hello from server";
    
    // Set port for server
    port = atoi(argv[1]);

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
      
    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                  &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
      
    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    
    /* Server goes in passive mode. Wakes up only when a connection
     * request is made
     */
    if (listen(server_fd, 10) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // Server continues to listen for new connections.
    while(1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                           (socklen_t*)&addrlen))<0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        /*Create child process. It handles a single client.*/
        int pid = fork();
        if (pid == 0) {
            /* Child process will stop listening for new connection
             * but will maintain the connection with this client.
             */
            close(server_fd);

            int trader_id = login(new_socket);
            if(trader_id == 0)      //Exit the process if login fails
                    exit(0);

            while(1) {
                valread = read(new_socket, buffer, 1024);
                printf("%s\n",buffer );
                send(new_socket , hello , strlen(hello) , 0 );
            }    
            exit(0);
        }
        else
            close(new_socket); //Parent closes the connection with client.
    }
    return 0;
}