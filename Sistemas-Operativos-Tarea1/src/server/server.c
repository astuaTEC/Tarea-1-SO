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
#include <time.h>

char consonantsMessage[1024];
char DIRECTORY[100];

/**
 * @brief Function to log a message in to the log file
 * 
 * @param tag identifier for the message
 * @param message the message to log
 */
void logger(const char *tag, const char *message)
{

    FILE *fp;
    time_t now;

    fp = fopen("logger.log", "a"); // open the file

    time(&now);
    fprintf(fp , "%s [%s]: %s\n", ctime(&now), tag, message); // append the message

    fclose(fp);
    
}

/**
 * @brief Function to print and log the errors
 * 
 * @param msg the error message
 */
void error(const char *msg)
{
    perror(msg);
    logger("ERROR", msg);
    exit(0);
}

/**
 * @brief function to read the system calls used to calculate the
 * consonats in a file using Strace
 * 
 * @param newSockfd socket number to send a message to client
 */
void readSysCalls(int newSockfd)
{

    int n;
    FILE *fp;
    char line[1024] = {0};
    char totalFile[24000]; // this is the final message to send to client
                           // with the syscalls and number of consonants

    fp = fopen("syscalls.txt", "r"); // 'r' opens the file in read mode

    if (fp == NULL)
    {
        perror("[-]Error in reading file.");
        exit(1);
    }

    while (fgets(line, 1024, fp) != NULL)
    {
        strcat(totalFile, line); // concat every file's line
        bzero(line, 1024);
    }

    strcat(totalFile, "\n");
    strcat(totalFile, consonantsMessage); // concat the number of consonants on a message format

    fclose(fp);

    n = write(newSockfd, totalFile, strlen(totalFile)); // send the message to client
    if (n < 0)
    {
        error("Error on writing");
    }

    logger("DATA", "Sending message to client");

    bzero(totalFile, strlen(totalFile));
    bzero(consonantsMessage, 1024);
    bzero(line, 1024);
}

/**
 * @brief function to save the file coming from the client
 * 
 * @param buffer the message from client
 * @param newSockfd socket number to send a message to client
 */
void write_file(char *buffer, int newSockfd)
{

    char copyBuffer[MAX_BUFFER], consonants[100];
    strcpy(copyBuffer, buffer); // a copy to modify separate

    FILE *fp;
    char *chunk = strtok(copyBuffer, ";"); // extract the file name from "<filename>;ETCETC..."

    char filename[50];
    bzero(filename, 50);

    strcat(filename, DIRECTORY); // Appends the directory to save the files
    strcat(filename, chunk); // appends the file name to directory

    fp = fopen(filename, "w");

    chunk = strtok(NULL, ";"); // extract the rest of the message (file content)

    fprintf(fp, "%s", chunk); // write the content

    logger("WRITING", "A new File was written");

    // Prepare the consonant counter message
    strcat(consonantsMessage, "The number of consonants in the the file are: ");
    sprintf(consonants, "%d", consonantCounter(chunk)); // call the function to count the consonats
    strcat(consonantsMessage, consonants);

    fclose(fp);

    char command[200];

    // prepare a command to get the system calls used by consonant counter function
    strcat(command, "strace -c -U total-time,calls,syscall -o syscalls.txt ./counter.out ");
    strcat(command, filename);

    system(command); // exec the command
    logger("SYSTEM", "Execution of Strace");

    readSysCalls(newSockfd); // call the function to read and send the syscalls to client

    bzero(copyBuffer, MAX_BUFFER);
    bzero(filename, 50);
    bzero(command, 200);
    bzero(chunk, strlen(chunk));
}

/**
 * @brief Function to open the server connection
 * 
 * @param argv argv[1]: port, argv[2]: directory to save the files
 * @return int state
 */
void openConnection(char *argv[])
{
    FILE *fp;

    fp = fopen("logger.log", "w");

    fprintf(fp, "%s", ""); // clear the logger

    fclose(fp);

    bzero(DIRECTORY, 100);
    strcat(DIRECTORY, argv[2]); // assign the directory

    mkdir(DIRECTORY, S_IRWXU);

    int sockfd, newSockfd, n;
    char inBuffer[MAX_BUFFER];

    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen;

    sockfd = socket(AF_INET, SOCK_STREAM, 0); // create the socket
    if (sockfd < 0)
    {
        error("Error opening socket");
    }

    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(IP);
    serv_addr.sin_port = atoi(argv[1]); // use the port from arguments

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        error("Binding Failed");
    }

    char str[INET_ADDRSTRLEN];
    printf("Ip %s  \n", inet_ntop(AF_INET, &(serv_addr.sin_addr), str, INET_ADDRSTRLEN));
    listen(sockfd, 5);

    clilen = sizeof(cli_addr);

    newSockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen); // accept a client
    logger("CONNECTION", "New client was acepted");

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
            newSockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen); // accept the client again if he was disconnected
            logger("CONNECTION", "New client was acepted");
        }

        if (strlen(inBuffer) > 0) // receive a message from client
        {
            logger("DATA", "Receiving message from client");
            write_file(inBuffer, newSockfd);
        }

        sleep(0.2);
    }

    close(sockfd);

}

int main(int argc, char *argv[])
{

    if(argc < 3){
        error("Missing arguments");
        return 1;
    }

    openConnection(argv);

    return 0;
}