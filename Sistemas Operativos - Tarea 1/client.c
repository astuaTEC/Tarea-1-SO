#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <ctype.h>

#define SIZE 1024


char ip[15]; // = "192.168.100.97";
int port; // = 8080;
int e;
int sockfd;
struct sockaddr_in server_addr;
FILE *fp;
char filename[40]; // = "send.txt";

// https://stackoverflow.com/questions/238603/how-can-i-get-a-files-size-in-c

int fsize(FILE *fp){
    int prev = ftell(fp);
    fseek(fp, 0L, SEEK_END);
    int sz = ftell(fp);
    fseek(fp, prev, SEEK_SET); // go back to where we were
    return sz;
}

void send_file(FILE *fp, int sockfd)
{
    int n, fileSize;

    fileSize = fsize(fp);

    char data[SIZE] = {0};

    char totalFile[fileSize];

    while (fgets(data, SIZE, fp) != NULL)
    {
        strcat(totalFile, data);
        bzero(data, SIZE);
    }

    if (send(sockfd, totalFile, sizeof(totalFile), 0) == -1)
        {
            perror("[-]Error in sending file.");
            bzero(totalFile, fileSize);
            exit(1);
        }

    bzero(totalFile, fileSize);
}

void checkFile(){

    printf("Ingrese el nombre del archivo: ");
    scanf("%s", filename);

    fp = fopen(filename, "r");
    if (fp == NULL)
    {
        perror("[-]Error in reading file.");
        checkFile();
    }

    send_file(fp, sockfd);
    printf("[+]File data sent successfully.\n");
    fp = NULL;
    
    char resp;

    while( strcmp(&resp, "y") != 0 && strcmp(&resp, "n") != 0) { 
        printf("Le gustaria enviar un archivo adicional? [y/n] ");
        scanf("%c", &resp);
    }

    if (strcmp(&resp, "y") == 0)
    {
        checkFile();
    }
    else if (strcmp(&resp, "n") == 0)
    {
        printf("[+]Closing the connection.\n");
        close(sockfd);
        return;
    }
}

void establishConnection(){
    printf("Ingrese la direccion IP: ");
    scanf("%15s", ip);

    printf("Ingrese el puerto: ");
    scanf("%d", &port);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("[-]Error in socket");
        // exit(1);
        establishConnection();
    }

    printf("[+]Server socket created successfully.\n");

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = port;
    server_addr.sin_addr.s_addr = inet_addr(ip);

    e = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (e == -1)
    {
        perror("[-]Error in socket");
        // exit(1);
        establishConnection();
    }

    printf("[+]Connected to Server.\n");
    checkFile();
}

int main()
{
    establishConnection();

    return 0;
}