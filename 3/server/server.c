#include <sys/socket.h> // socket(), bind(), listen(), accept(), read() & write()
#include <pthread.h>  // pthread_create() & pthread_exit()
#include <netinet/in.h> // for sockaddr_in structure
#include <string.h> // for bzero() & strcmp()
#include <arpa/inet.h> // for inet_addr()
#include <unistd.h> // for close()
#include <stdlib.h> // for exit()
#include <stdio.h> // for printf() & scanf()

#define MAX_CLIENTS 5

// fucntion called while the program exits
#pragma exit closeSocket

struct User {
    int id;
    int sock_fd;
    struct sockaddr_in address;
};

int connfd, sockfd;

void *connection(void *arg) {
    struct User *thread = (struct User *) arg;
    int sock_fd = thread->sock_fd;
    char str[20];
    while(1) {
        int bytes = recv(sock_fd, str, sizeof(str), 0);
        if (bytes <= 0) {
            break;
        }
        str[bytes] = '\0';

        if(strcmp("quit\n", str) == 0) {
            printf("\nClient %d disconnected\n\n", thread->id);
            break;
        }
        
        printf("Client %d : %s", thread->id, str);
    }
    close(sock_fd);
    pthread_exit(NULL);
}


int main() {    
    char IP[20];
    int port;
    int len, user_cnt = 0;
    pthread_t users[MAX_CLIENTS];
    struct User user_data[MAX_CLIENTS];
    struct sockaddr_in server, client[MAX_CLIENTS];

    // taking input
    printf("Enter the IP address : ");
    scanf("%s", IP);
    printf("Enter the port number : ");
    scanf("%d", &port);

    sockfd = socket(AF_INET,SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("\nFailed to create the server\n");
        exit(0);
    }

    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(IP);
    server.sin_port = htons(port);

    // creating the server
    if ((bind(sockfd, (struct sockaddr*) &server, sizeof(server))) != 0) {
        printf("\nFailed to create the server\n");
        exit(0);
    } else {
        printf("\nServer created successfully\nIP Address : %s\nPort number : %d\n\n", IP, port);
    }
        
    while(1)
    {
        if ((listen(sockfd, MAX_CLIENTS)) != 0) {
            printf("An error occured!\n");
            exit(0);
        } else {
            printf("Server is waiting for clients...\n\n");
        }
        
        // handling clients
        len = sizeof(client[user_cnt]);
        connfd = accept(sockfd, (struct sockaddr*) &client[user_cnt], &len);
        if (connfd < 0) {
            printf("\nClient failed to connect\n");
        } else {
            printf("\nClient %d connected successfully\n\n", user_cnt + 1);
        }

        user_data[user_cnt].id = user_cnt + 1;
        user_data[user_cnt].sock_fd = connfd;
        user_data[user_cnt].address = client[user_cnt];

        // creating a thread for each client
        int stat = pthread_create(&users[user_cnt], NULL, connection, &user_data[user_cnt]);
        user_cnt++;

        if(stat) {
            fprintf(stderr,"Failed to create a thread!\n");        
        }

        // handling maximum connections
        if(user_cnt > MAX_CLIENTS) {
            break;
        }
    }   
    exit(0);
}

void closeSocket() {
    close(sockfd);
}