#include <sys/socket.h> // for socket(), connect(), send() & recv()
#include <arpa/inet.h> // for inet_addr()
#include <string.h> // for bzero() & strcmp()
#include <unistd.h> // for close()
#include <stdlib.h> // for exit()
#include <stdio.h> // for printf() & scanf()

void client(int sockfd) {
    char buffer[20];
    int c;

    while ((c = getchar()) != '\n' && c != EOF)
    bzero(buffer, sizeof(buffer));  

    // taking input for data to be sent
    while(1) {  
        printf("Enter the string to be sent : ");
        int ind = 0;
        bzero(buffer, sizeof(buffer));  
        while ((buffer[ind++] = getchar()) != '\n');

        write(sockfd, buffer, sizeof(buffer));

        // handling quit
        if ((strcmp(buffer, "quit\n")) == 0) {
            printf("\nClient disconnected\n");
            break;
        }
    }
}

int main() {
    char IP[20];
    int port;
    int sockfd;
    struct sockaddr_in server;
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("Failed to create socket\n");
        exit(0);
    }

    // taking input
    printf("Enter the IP Address : ");
    scanf("%s", IP);
    printf("Enter the port number : ");
    scanf("%d", &port);
    bzero(&server, sizeof(server));

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(IP);
    server.sin_port = htons(port);
    
    // connect to server
    if (connect(sockfd, (struct sockaddr*) &server, sizeof(server)) != 0) {
        printf("\nFailed to connect to the server!\n");
        exit(0);
    } else {
        printf("\nConnected to the server successfully\n\n");
    }
        
    client(sockfd);
}