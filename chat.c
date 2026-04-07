#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>



#define BLEU "\033[34m"
#define ROUGE "\033[31m"
#define RESET "\033[0m"

void *routine_client(){
 int *socketA;
    socketA = malloc(sizeof(*socketA));
    if(socketA == NULL){
        fprintf(stderr, "Cannot allocate memory \n");
        exit(EXIT_FAILURE);
    }

    *socketA = socket(AF_INET, SOCK_STREAM, 0);
    if(*socketA == -1){
        fprintf(stderr, "Cannot create socket: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in sockAddress;
    memset(&sockAddress, 0, sizeof(sockAddress));
    const char *server_IP = "0.0.0.0";
        sockAddress.sin_family = AF_INET;
        sockAddress.sin_port = htons(8080);
    if(inet_pton(AF_INET, server_IP, &sockAddress.sin_addr) <= 0){
        fprintf(stderr, "Invalide address \n");
        close(*socketA);
        exit(EXIT_FAILURE);
    }

    int length = sizeof(sockAddress);
    while(connect(*socketA, (const struct sockaddr*)&sockAddress, (socklen_t)length) == -1){
        fprintf(stderr, "Cannot connect to server: %s\n", strerror(errno));
        sleep(1);
    }

    return (void*)socketA;
}


void *routine_serveur(void *arg){
   int *socketA;
    socketA = malloc(sizeof(*socketA));
    if(socketA == NULL){
        fprintf(stderr, "Cannot allocate memory \n");
        exit(EXIT_FAILURE);
    }

    *socketA = socket(AF_INET, SOCK_STREAM, 0);
    if(*socketA == -1){
        fprintf(stderr, "Cannot create socket: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in sockAddress;
    memset(&sockAddress, 0, sizeof(sockAddress));
        sockAddress.sin_family = AF_INET;
        sockAddress.sin_port = htons(8080);
        sockAddress.sin_addr.s_addr = INADDR_ANY;

    int length = sizeof(sockAddress);
    if(bind(*socketA, (const struct sockaddr*)&sockAddress, (socklen_t)length) == -1){
        fprintf(stderr, "Cannot connect bind: %s\n", strerror(errno));
        close(*socketA);
        exit(EXIT_FAILURE); 
    }
    if(listen(*socketA, 1) == -1){
        fprintf(stderr, "Cannot listen to incoming connection: %s\n", strerror(errno));
        close(*socketA);
        exit(EXIT_FAILURE);   
    }
    int *socketB;
    socketB = malloc(sizeof(*socketB));
    if(socketA == NULL){
        fprintf(stderr, "Cannot allocate memory \n");
        close(*socketA);
        exit(EXIT_FAILURE);
    }
    *socketB = accept(*socketA, (struct sockaddr*)&sockAddress, (socklen_t*)&length);
    if(*socketB == -1){
        fprintf(stderr, "Cannot accept connections: %s\n", strerror(errno));
        close(*socketA);
        exit(EXIT_FAILURE);    
    }
    puts("Listening \n");
    return (void*)socketB;
}

void *routine_sending_data(void *arg){
    int *socket = (int*)arg;
    char message[1024];
    while (1)
    {
        memset(message, 0, sizeof(message));
        if(fgets(message, 1024, stdin) == NULL){
            break;
        }
        if (send(*socket, message, sizeof(message) - 1, 0) < 0)
        {
            fprintf(stderr, "Cannot send message: %s", strerror(errno));
            close(*socket);
            exit(EXIT_FAILURE);
        }
        
    }
    
}

void *routine_receiving_data(void *arg){
    int *socket = (int*)arg;
    char buffer[1024];
    while (1)
    {
        if(recv(*socket, buffer, sizeof(buffer) - 1, 0) <= 0){
            fprintf(stderr, "Cannot receive message: %s", strerror(errno));
            close(*socket);
            exit(EXIT_FAILURE);
        }
        printf(BLEU"\n%s\n"RESET, buffer);
    }
}
 

int main(int argc, char *argv[]){
    pthread_t threadClient; //create socket used to connect to another user to send him messages.
    pthread_t threadServeur; //Create socket that listen to incoming connections;
    pthread_t sending_data_Client;
    pthread_t receiving_data_serveur;

    int *socketClient;
    int *socketServeur;

    if(pthread_create(&threadClient, NULL, &routine_client, NULL) != 0){
        fprintf(stderr, "Cannot create thread: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    if(pthread_create(&threadServeur, NULL, &routine_serveur, NULL) != 0){
        fprintf(stderr, "Cannot create thread: %s", strerror(errno));
        exit(EXIT_FAILURE);   
    }
    if(pthread_join(threadClient, (void**)&socketClient)){
        fprintf(stderr, "Cannot join thread: %s", strerror(errno));
        exit(EXIT_FAILURE);  
    }
    if(pthread_join(threadServeur, (void**)&socketServeur)){
        fprintf(stderr, "Cannot join thread: %s", strerror(errno));
        exit(EXIT_FAILURE);  
    }

    //thread in charge of sending data
    if(pthread_create(&sending_data_Client, NULL, &routine_sending_data, socketClient) != 0){
        fprintf(stderr, "Cannot create tthread: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    if(pthread_create(&receiving_data_serveur, NULL, &routine_receiving_data, socketServeur) != 0){
        fprintf(stderr, "Cannot create thread: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    if(pthread_join(sending_data_Client, NULL)){
        fprintf(stderr, "Cannot join thread: %s", strerror(errno));
        exit(EXIT_FAILURE);  
    }
    if(pthread_join(receiving_data_serveur, NULL)){
        fprintf(stderr, "Cannot join thread: %s", strerror(errno));
        exit(EXIT_FAILURE);  
    }

    return 0;
}

