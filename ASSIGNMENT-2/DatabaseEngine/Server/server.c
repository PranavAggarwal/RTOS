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
char *server_func(char filename[]){
    FILE *f;
    char *final = "";
    
    // Check for invalid instruction
    if(filename[0] != 'G' || filename[1] != 'E' || filename[2] != 'T' || filename[3] != ' '){
        final = "Invalid Instruction";
        strcat(final,"\0");
        printf("%s\n", final);
        return final;
    }
    
    // Extracting the filename
    int len = strlen(filename);
    int i;
    for(i = 4; i <= len; i++){
        filename[i-4] = filename[i];
    }
    printf("Filename is: %s\n", filename);

    // Checking if ifle exist
    f = fopen(filename,"r");
    if(f == NULL){
        final = "Data not found";
        strcat(final,"\0");
        printf("%s\n", final );
        return final;
    }
    
    // Storing data in a string
    char message[length] = "", str[length];
    while(fscanf(f, " %[^\n]", str) != EOF){
        strcat(message, strcat(str,"\n"));
    }   

    strcat(message,"\0");
    final = message;
    printf("%s\n", final);
    fclose(f);
    return final;
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
                
                // Sending the data nack to client
                write(new_socket,buffer,strlen(buffer));
            }
        }
        else{
            // Closing the socket for client
            close(new_socket);
        }
    }

    return 0;
}