#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include "config.h"
#include "consonantCounter.h"

char consonantsMessage[1024];

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

void readSysCalls(int newSockfd)
{

    int n;
    FILE *fp;
    char line[1024] = {0};
    char totalFile[24000];

    fp = fopen("syscalls.txt", "r"); // 'r' opens the file in read mode

    if (fp == NULL)
    {
        perror("[-]Error in reading file.");
        exit(1);
    }

    while (fgets(line, 1024, fp) != NULL)
    {
        strcat(totalFile, line);
        bzero(line, 1024);
    }

    strcat(totalFile, "\n");
    strcat(totalFile, consonantsMessage);

    n = write(newSockfd, totalFile, strlen(totalFile));
    if (n < 0)
    {
        error("Error on writing");
    }

    bzero(totalFile, strlen(totalFile));
    bzero(consonantsMessage, 1024);
    bzero(line, 1024);
}

void write_file(char *buffer, int newSockfd)
{

    char copyBuffer[MAX_BUFFER], consonants[100];
    strcpy(copyBuffer, buffer);

    FILE *fp;
    char *chunk = strtok(copyBuffer, ";");

    char filename[50];
    bzero(filename, 50);

    strcat(filename, DIRECTORY);
    strcat(filename, chunk);

    fp = fopen(filename, "w");

    chunk = strtok(NULL, ";");

    fprintf(fp, "%s", chunk);

    strcat(consonantsMessage, "The number of consonants in the the file are: ");
    sprintf(consonants, "%d", consonantCounter(chunk));
    strcat(consonantsMessage, consonants);

    fclose(fp);

    char command[200];

    strcat(command, "strace -c -U total-time,calls,syscall -o syscalls.txt ./counter.out ");
    strcat(command, filename);

    system(command);

    readSysCalls(newSockfd);

    bzero(copyBuffer, MAX_BUFFER);
    bzero(filename, 50);
    bzero(command, 200);
    bzero(chunk, strlen(chunk));
}

int main(int argc, char *argv[])
{

    // if (argc < 2){
    //     fprintf(stderr, "Port not provided. Program terminated\n");
    //     exit(1);
    // }
    mkdir(DIRECTORY, S_IRWXU);

    int sockfd, newSockfd, n;
    char inBuffer[MAX_BUFFER];

    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        error("Error opening socket");
    }

    bzero((char *)&serv_addr, sizeof(serv_addr));
    // port = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(IP);
    serv_addr.sin_port = PORT;

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        error("Binding Failed");
    }

    char str[INET_ADDRSTRLEN];
    printf("Ip %s  \n", inet_ntop(AF_INET, &(serv_addr.sin_addr), str, INET_ADDRSTRLEN));
    listen(sockfd, 5);

    clilen = sizeof(cli_addr);

    newSockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);

    if (newSockfd < 0)
    {
        error("Error on Accept");
    }

    while (1)
    {

        bzero(inBuffer, MAX_BUFFER);
        n = read(newSockfd, inBuffer, MAX_BUFFER);

        if (n < 0)
        {
            error("Error on reading");
        }

        else if (n == 0)
        {
            printf("Closing connection...\n");
            newSockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
        }

        if (strlen(inBuffer) > 0)
        {
            write_file(inBuffer, newSockfd);
            // n = write(newSockfd, consonants, strlen(consonants));
            // if (n < 0)
            // {
            //     error("Error on writing");
            // }
        }

        sleep(0.2);
    }

    close(sockfd);

    return 0;
}