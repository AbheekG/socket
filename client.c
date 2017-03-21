// Client side C/C++ program to demonstrate Socket programming
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

const char *server_ip;
int port;

int login(int sock)
{
    char username[10] = {0}, password[100] = {0}, buffer[1024] = {0};
   
    printf("Login\n");
    printf("Username: ");
    scanf("%s", username);
    printf("Password: ");
    scanf("%s", password);

    strcat(username, " ");
    strcat(username, password);
    
    
    send(sock, username, strlen(username), 0);
    read(sock, buffer, 1024);
    
    printf("%s\n", buffer);
    
    if (!strcmp(buffer, "No")) {
        printf("Login Failed\n");
        return 0;
    }
    printf("Login Successfull\n");
    return 1;
}

void order_status(int sock)
{
    char buffer[1024] = {0};

    read(sock, buffer, 1024);
    // printf("\n\naaa%s\n\n", buffer);
    if (!strcmp(buffer, "end1"))
        printf("\nNo Sell Orders\n");
    else
        printf("\nSell orders\n===========\n");

    
    while(strcmp(buffer, "end1")) {
        // printf("\nHERE %s\n", buffer);   
        char *sid = strtok(buffer, " ");
        char *price = strtok(NULL, " ");
        char *quantity = strtok(NULL, " ");
        printf("Item Code %d \t | Best Price %s \t | Quantity %s |\n", atoi(sid), price, quantity);
        memset(buffer, 0, 1024);
        read(sock, buffer, 1024);
        // printf("\n\nbbb%s\n\n", buffer);
    }
    memset(buffer, 0, 1024);
    read(sock, buffer, 1024);
    
    // printf("\n\nccc%s\n\n", buffer);
    
    if (!strcmp(buffer, "end2"))
        printf("\nNo Buy Orders\n");
    else
        printf("\nBuy orders\n============\n");

    while(strcmp(buffer, "end2")) {
        char *sid = strtok(buffer, " ");
        char *price = strtok(NULL, " ");
        char *quantity = strtok(NULL, " ");
        printf("Item Code %d \t | Best Price %s \t | Quantity %s |\n", atoi(sid), price, quantity);
        memset(buffer, 0, 1024);
        read(sock, buffer, 1024);
    }

}

int trade_status(int sock)
{
    char buffer[1024] = {0};

    read(sock, buffer, 1024);
    while(strcmp(buffer, "end")) {
        char *a = strtok(buffer, " ");
        char *f = strtok(NULL, " ");
        char *b = strtok(NULL, " ");
        char *c = strtok(NULL, " ");
        char *d = strtok(NULL, " ");
        char *e = strtok(NULL, " ");

        printf("\n%s %s\n----------\n", a, f);
        printf("Item Code %d \t | Price %s \t | Quantity %s \t | Counterparty_id %s |\n",
                    atoi(b)+1, c, d, e);
        memset(buffer, 0, 1024);
        read(sock, buffer, 1024);
    }
}

int main(int argc, char const *argv[])
{
    struct sockaddr_in address;
    int sock = 0, valread;
    
    // Set the server's port and ip address
    port = atoi(argv[2]);
    server_ip = argv[1];

    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    // char hello[1024];
    while(1) {
        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
            printf("\n Socket creation error \n");
            return -1;
        }

        memset(&serv_addr, '0', sizeof(serv_addr));
      
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(port);

        // Convert IPv4 and IPv6 addresses from text to binary form
        if(inet_pton(AF_INET, server_ip, &serv_addr.sin_addr)<=0) 
        {
            printf("\nInvalid address/ Address not supported \n");
            return -1;
        }

        if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        {
            printf("\nNo server \n");
            return -1;
        }

        if (!login(sock))       //If login fails then reconnect with server
            continue;

        while(1) {
            char hello[1024] = {0};
            printf("Request: ");
            memset(hello, 0, sizeof(hello));           
            scanf("%s", hello);
            send(sock, hello, strlen(hello), 0);
            printf("Request sent\n");
            if (!strcmp(hello, "Exit"))
                return 0;

            if(!strcmp(hello, "Order_Status"))
                order_status(sock);
            else if(!strcmp(hello, "Trade_Status"))
                trade_status(sock);

            memset(buffer, 0, sizeof(buffer));
            valread = read( sock , buffer, 1024);
            if (!valread) {
                printf("Connection lost\n");
                break;
            }
            if(strcmp(buffer, "done"))
                printf("%s\n", buffer);
        }
    }
    return 0;
}

