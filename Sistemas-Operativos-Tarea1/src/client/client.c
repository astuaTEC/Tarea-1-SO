#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <pthread.h>

#define SIZE 4096

char ip[15]; // "192.168.100.97";
int port; // 8080;
int e;
int sockfd;
struct sockaddr_in server_addr;
FILE *fp;
char filename[50]; // "send.txt";
pthread_t t1, t2; // threads are to execute send and recive simultaneusly

/**
 * @brief Function to get a file's size
 * @param fp file to get the size
 * Reference: / https://stackoverflow.com/questions/238603/how-can-i-get-a-files-size-in-c
 */
int fsize(FILE *fp){
    int prev = ftell(fp);
    fseek(fp, 0L, SEEK_END);
    int sz = ftell(fp);
    fseek(fp, prev, SEEK_SET); // go back to where we were
    return sz;
}

/**
 * @brief function to send a file to server
 * 
 * @param fp the file
 * @param sockfd socket number to sent the message
 * @param name file's name
 */
void send_file(FILE *fp, int sockfd, char *name)
{
    int n, fileSize;

    fileSize = fsize(fp); // get the size

    char data[SIZE] = {0};

    char totalFile[fileSize+50]; // 50: Fit for the name

    strcat(totalFile, strcat(name, ";")); // Concat the file name in format: "<filename>;"

    while (fgets(data, SIZE, fp) != NULL)
    {
        strcat(totalFile, data); // concat every file's line
        bzero(data, SIZE);
    }

    if (send(sockfd, totalFile, sizeof(totalFile), 0) == -1) // send the message
        {
            perror("[-]Error in sending file.");
            bzero(totalFile, fileSize);
            exit(1);
        }

    bzero(totalFile, fileSize);
}

/**
 * @brief Function to validate if the file exists
 * 
 * @return void* 
 */
void *checkFile(){

    bzero(filename, 50);
    printf("Ingrese el nombre del archivo: "); // the file name is requested
    scanf("%s", filename);

    fp = fopen(filename, "r");
    if (fp == NULL) // check if the file exist
    {
        perror("[-]Error in reading file.");
        checkFile(); // if not, call the same function
    }

    send_file(fp, sockfd, filename); // send the message to server
    printf("[+]File data sent successfully.\n");
    fp = NULL;
    
    char resp;
    bzero(&resp, 1);

    printf("Le gustaria enviar un archivo adicional? [y/n]\n"); // if the user need to send another file
    scanf("%c", &resp);

    while (strcmp(&resp, "y") != 0 & strcmp(&resp, "n") != 0) // only accepts letter "y" or "n"
    {

        printf("Le gustaria enviar un archivo adicional? [y/n]\n");
        scanf("%c", &resp);
        sleep(1);
    }

    if (strcmp(&resp, "y") == 0)
    {
        bzero(&resp, 1);
        checkFile(); // the user need to send another file
    }

    else if (strcmp(&resp, "n") == 0)
    {
        printf("[+]Closing the connection.\n");
        close(sockfd); // close the socket
        bzero(&resp, 1);

        // close the threads
        pthread_cancel(t1);
        pthread_cancel(t2);
    }
    
}

/**
 * @brief Function to receive a message from the server
 * 
 * @return void* 
 */

void *receiveMessage(){
    int n;
    char buffer[4096]; 
    while(1){

        n = read(sockfd, buffer, 4096);
        if(n < 0){
            perror("Error on reading");
        }

        if(strlen(buffer) > 0){ //if receive some message
            printf("%s\n", buffer);
            bzero(buffer, 4096);
        }

        sleep(0.5);
    }
}

/**
 * @brief Funtion to establish connection to server
 * 
 */
void establishConnection(){
    printf("Ingrese la direccion IP: "); // request the ip address
    scanf("%15s", ip);

    printf("Ingrese el puerto: "); // request the port
    scanf("%d", &port);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("[-]Error in socket");
        // if the connection fail, calls the function again 
        establishConnection();
    }

    printf("[+]Server socket created successfully.\n");

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = port;
    server_addr.sin_addr.s_addr = inet_addr(ip);

    e = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (e < 0)
    {
        perror("[-]Error in socket");
        // if the socket fail, calls the function again 
        establishConnection();
    }

    printf("[+]Connected to Server.\n");

    // create a thread to send data, and other to receive data
    pthread_create(&t1, NULL, checkFile, NULL);
    pthread_create(&t2, NULL, receiveMessage, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL); 
}

int main()
{
    establishConnection();

    return 0;
}