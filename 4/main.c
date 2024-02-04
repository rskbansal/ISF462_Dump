#include <sys/socket.h> // for socket(), connect(), send() & recv()
#include <netinet/in.h> // for struct sockaddr_in
#include <arpa/inet.h> // for inet_addr()
#include <netdb.h> // for struct hostent
#include <string.h> // for strchr(), strndup() & memset()
#include <stdio.h> // for printf() & sprintf()
#include <stdlib.h> // for exit()
#include <unistd.h> // for close()

#define h_addr h_addr_list[0] // for backward compatibility
#define BUFFER_SIZE 4096

int main(int argc, char *argv[]) {
    if(argc != 2) {
        // how to use the program
        fprintf(stderr, "Usage : %s <URL>\n", argv[0]);
        exit(1);
    }

    char *url = argv[1];
    char *host, *path;
    char *slash = strchr(url, '/');
    if (slash != NULL) {
        // found a forward slash, so the hostname is the substring before it
        host = strndup(url, slash - url);
        path = slash;
    } else {
        // no forward slash found, so the entire URL is the hostname
        host = url;
        path = "/";
    }

    struct sockaddr_in address;
    char request[BUFFER_SIZE], response[BUFFER_SIZE];
    struct hostent *server = gethostbyname(host);
    if(server == NULL) {
        printf("Error resolving hostname\n");
        exit(1);
    }

    // creating socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0) {
        printf("Error creating socket\n");
        exit(1);
    }
    
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(80);

    // establishing connection
    memcpy(&address.sin_addr.s_addr, server->h_addr, server->h_length);
    int connection = connect(sockfd, (struct sockaddr*)&address, sizeof(address));
    if(connection < 0) {
        printf("Error connecting to server\n");
        exit(1);
    }

    // sending the GET request
    sprintf(request, "GET %s HTTP/1.0\r\nHost: %s\r\n\r\n", path, host);
    if(send(sockfd, request, strlen(request), 0) < 0) {
        printf("Could not GET HTTP request\n");
        exit(1);
    }

    int check = 0;
    int bytes = 0;
    int end = -1;

    // removing the HTTP header to get only the HTML content
    while((bytes = recv(sockfd, response, sizeof(response), 0)) > 0) {
        for(int i = 0; i < bytes - 3; i++) {
            if(response[i] == '\r' && response[i+1] == '\n' && response[i+2] == '\r' && response[i+3] == '\n') {
                end = i + 4;
                break;
            }
        }

        // writing response to HTML file
        if(end >= 0) {
            FILE *fp = fopen("test.html", "w");
            fwrite(response + end, 1, bytes - end, fp);
            fclose(fp);
            check = 1;
        }
    }

    if(check ==1) {
        printf("Succesful GET on %s and contents saved in test.html\n\n", argv[1]);
    }

    close(sockfd);
    return 0;
}
