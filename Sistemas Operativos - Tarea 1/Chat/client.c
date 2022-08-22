#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>

void error(const char *msg){
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[]){

    int sockfd, port, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[255];
    char inBuffer[255];

    if (argc < 3){
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(1);
    }

    port = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0){
        error("ERROR apening socket");
    }

    server = gethostbyname(argv[1]);
    if(server == NULL){
        fprintf(stderr, "Error, no such host");
    }

    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *) server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = port;
    
    if(connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
        error("Connection failed");
    }

    while (1)
    {
        bzero(buffer, 255);

        fgets(buffer, 255, stdin);
        n = write(sockfd, buffer, strlen(buffer));
        if(n < 0){
            error("Error on writing");
        }

        int i = strncmp("Bye", buffer, 3);
        if(i == 0){
            break;
        }
        
        bzero(inBuffer, 255);
        n = read(sockfd, inBuffer, 255);
        if(n < 0){
            error("Error on reading");
        }
        
        i = strncmp("Bye", inBuffer, 3);
        if(i == 0){
            break;
        }

        printf("Server: %s", inBuffer);
        
    }

    close(sockfd);
    return 0;
    
}

