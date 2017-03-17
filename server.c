// Server side C program to demonstrate Socket programming
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "list.h"

int port, new_socket;
char *trader_pass[10] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10"};

int login(int new_socket)
{
    char buffer[1024] = {0}, ack[1024] = {0};
    memset(buffer, 0, 1024);
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

void process(int new_socket, int trader_id) {
    char buffer[1024] = {0};
    memset(buffer, 0, 1024);
    int valread = read(new_socket, buffer, 1024);
    printf("%s\n", buffer);

    if(!strcmp(buffer, "Buy\0") || !strcmp(buffer, "Sell\0"))
    {
        int id;
        if (!strcmp(buffer, "Buy\0"))
            id = 1;
        else
            id = 2;
        struct order *t = malloc(sizeof(struct order));

       // struct order *t = mmap(NULL, sizeof(struct order), PROT_READ | PROT_WRITE, 
       //              MAP_SHARED | MAP_ANONYMOUS, -1, 0);

        t->trader_id = trader_id;
        char *str = "Item Code\0";
        send(new_socket , str , strlen(str) , 0 );
        memset(buffer, 0, 1024);
        read(new_socket, buffer, 1024);
        sscanf(buffer, "%d", &t->item_code);
        str = "Quantity\0";
        send(new_socket , str , strlen(str), 0 );
        memset(buffer, 0, 1024);
        read(new_socket, buffer, 1024);
        sscanf(buffer, "%d", &t->quantity);
        str = "Price\0";
        send(new_socket , str , strlen(str), 0 );
        memset(buffer, 0, 1024);
        read(new_socket, buffer, 1024);
        sscanf(buffer, "%d", &t->price);
        t->next = NULL;
        t->prev = NULL;
        t->item_code = t->item_code-1;
        printf("AD: %s\n\n", buffer);
        insert_order(id, t);

    }

    else if(!strcmp(buffer, "Order_Status\0"))
        order_status(new_socket);
    else if(!strcmp(buffer, "Trade_Status\0"))
        trade_status(trader_id, new_socket);
    else
        printf("Wrong input: %s\n", buffer);

    char *str = "done\0";
    sleep(1);
    send(new_socket , str , strlen(str), 0 );
}

void* start_conn(void *temp)
{
    int new_socket = *((int*)temp);
    int trader_id = login(new_socket);

    if(trader_id == 0)      //Exit the process if login fails
        return NULL;

    while(1)
        process(new_socket, trader_id);
}

int main(int argc, char const *argv[])
{
    int server_fd;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
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

    /* Server continues to listen for new connections. */
    pthread_t threads[5];
    int t = 0;
    while(1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                           (socklen_t*)&addrlen))<0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        /*Create child thread. It handles a single client.*/
        int rc = pthread_create(&threads[t], NULL, start_conn, (void *)&new_socket);
        if (rc){
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
        t = t + 1;        
    }
    while(t > 0) {
        if(pthread_join(threads[t-1], NULL)) {
            fprintf(stderr, "Error joining thread\n");
            return 2;
        }
        t--;
        printf("Joined thread %d\n", t);
    }

    return 0;
}
