#include<sys/types.h>
#include<sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <stdlib.h>
#include "pthread.h"
#include <stddef.h>
#include <pulse/simple.h>
#include <pulse/error.h>

#define BUFSIZE 1024
#define length 1024

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

int main(int argc, char const *argv[])
{
    // RECORDING SETUP

    // The sample type to use
    static const pa_sample_spec ss = {
        .format = PA_SAMPLE_S16LE,
        .rate = 20000,
        .channels = 2
    };

    pa_simple *s = NULL;
    int ret = 1;
    int error;
    
    /* Create the recording stream */
    if (!(s = pa_simple_new(NULL,"Julius", PA_STREAM_PLAYBACK, NULL, "playback", &ss, NULL, NULL, &error))) 
    {
        fprintf(stderr, __FILE__": pa_simple_new() failed: %s\n", pa_strerror(error));
        if (s){
            pa_simple_free(s);    
        }
        //return ret;
    }

    // SERVER SETUP
    int server_socket, new_socket;
    struct sockaddr_in serv_add, client_add;
    int addrlen = sizeof(serv_add);
    
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
    if(listen(server_socket, 2) < 0){
        perror("Listen error");
        exit(1);
    }

    printf("Waiting for connection with client\n");
    while(1){
        // Accepting client connection
        if((new_socket = accept(server_socket, (struct sockaddr *)&serv_add, (socklen_t*)&addrlen)) >= 0){
            printf("Request accepted\n");
            while(1){
                // Receing data from client
                uint8_t buf[BUFSIZE];
                ssize_t r;

                int val = read(new_socket, buf, sizeof(buf));
                printf("Read%d\n", val);
                
                // Playing the recorded message
                if (pa_simple_write(s, buf, sizeof(buf), &error) < 0) {
                    fprintf(stderr, __FILE__": pa_simple_write() failed: %s\n", pa_strerror(error));
                }
            }
        }    
    }
    return 0;
}