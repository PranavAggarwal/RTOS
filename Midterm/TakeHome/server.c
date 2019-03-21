#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define length 1024
// Server function
char *server_func(char server_no[]){
    // Creating filename depending on sensor number
    char filename[length];
    strcpy(filename, "");
    strcat(filename,"sensor");
    strcat(filename, server_no);
    strcat(filename, ".txt");

    // opening the sensor file
    FILE *f;
    f = fopen(filename, "r");

    // Getting the sensor data
    char message[length] = "", str[length];
    while(fscanf(f, " %s", str) != EOF){
        strcat(message, str);
    }   

    strcat(message,"\0");
    fclose(f);

    // Sending the data back
    char *data = message;
    return data;
}

int main(int argc, char const *argv[])
{
    int server_socket, new_socket;
    struct sockaddr_in serv_add, client_add;
    int addrlen = sizeof(serv_add);
    char buff[length];
    char *buffer;
    
    // Creating the socket
    printf("Creating the socket\n");
    if((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("Socket error");
        exit(1);
    }
    
    //Setting up the address 
    memset(&serv_add, '\0', sizeof(serv_add));  
    serv_add.sin_family = AF_INET;
    serv_add.sin_addr.s_addr = INADDR_ANY;
    serv_add.sin_port = htons(8000);
    
    // Binding server address to the socket
    printf("binding\n");  
    if((bind(server_socket, (struct sockaddr *)&serv_add,sizeof(serv_add))) < 0){
        perror("Bind error");
        exit(1);
    }

    // Listening to clients
    printf("listening\n");
    if(listen(server_socket, 5) < 0){
        perror("Listen error");
        exit(1);
    }

    // Accepting clients and responding back to them
    while(1){
        // Accepting connections from client
        printf("Waiting for connection with client\n");
        if((new_socket = accept(server_socket, (struct sockaddr *)&serv_add, (socklen_t*)&addrlen)) < 0){
            perror("Accept error");
            exit(1);
        }
        printf("Request accepted\n");

        // Creating child process for any new client request
        if(!fork()){
            // Closing the server socket
            close(server_socket);

            // Message sending and receiving
            while(1){
                // Receing data from client
                printf("\nReceiving request message from client\n");
                read(new_socket, buff, sizeof(buff));
                printf("Request from client:%s\n",buff);

                // Data to be sent back to client
                buffer = server_func(buff);
                
                // Sending the data back to client
                write(new_socket, buffer, strlen(buffer));
            }
        }
        else{
            // Closing the socket for client
            close(new_socket);
        }
    }

    return 0;
}