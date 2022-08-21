#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

void error(const char *msg){
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[]){

    if (argc < 2){
        fprintf(stderr, "Port not provided. Program terminated\n");
        exit(1);
    }

    int sockfd, newSockfd, port, n;
    char buffer[65000];

    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if( sockfd < 0){
        error("Error opening socket");
    }

    bzero((char *)&serv_addr, sizeof(serv_addr));
    port = atoi(argv[1]);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("192.168.100.4");
    serv_addr.sin_port = port;

    if(bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr) ) < 0){
        error("Binding Failed");
    }

    char str[INET_ADDRSTRLEN];
    printf("Ip %s  \n", inet_ntop(AF_INET, &(serv_addr.sin_addr), str, INET_ADDRSTRLEN));
    listen(sockfd, 5);
    
    clilen = sizeof(cli_addr);

    newSockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);

    if(newSockfd < 0){
        error("Error on Accept");
    }

    while (1)
    {
        bzero(buffer, 65000);
        n = read(newSockfd, buffer, 65000);
        if(n < 0){
            error("Error on reading");
        }

        printf("Client: %s", buffer);
        bzero(buffer, 255);
        fgets(buffer, 255, stdin);

        n = write(newSockfd, buffer, strlen(buffer));
        if(n < 0){
            error("Error on writing");
        }

        int i = strncmp("Bye", buffer, 3);
        if (i == 0){
            break;
        }
    }

    close(newSockfd);
    close(sockfd);
    return 0;

}