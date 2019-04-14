#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>
#include "pthread.h"
#include <stddef.h>
#include <pulse/simple.h>
#include <pulse/error.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#define PORT 8000
#define BUFSIZE 1024

int main(int argc, char *argv[]){
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
    if (!(s = pa_simple_new(NULL, "rec", PA_STREAM_RECORD, NULL, "record", &ss, NULL, NULL, &error))) {
        fprintf(stderr, __FILE__": pa_simple_new() failed: %s\n", pa_strerror(error));
        if (s){
	        pa_simple_free(s);
	    }
    }

	// SOCKET SETUP
	int cli_sock;
	struct sockaddr_in serv_add;

	// Creating the socket
	printf("Creating the socket\n");
	if((cli_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		perror("Socket error");
		exit(1);
	}

	// Setting server address
	printf("Setting server address\n");
	memset(&serv_add, 0, sizeof(serv_add));
	serv_add.sin_family = AF_INET;
    serv_add.sin_port = htons(8000);
    // Adding the servers IP
    if(inet_pton(AF_INET, argv[1], &serv_add.sin_addr)<=0){
    	perror("pton error");
    } 

    // Sending the connection request to server
	printf("Waiting for connection\n");
    if((connect(cli_sock, (struct sockaddr *)&serv_add, sizeof(serv_add))) < 0){
    	perror("Connection failed");
    	exit(1);
    }
    printf("Connection established\n\n");

    // Sending recorded data
    while(1){
		uint8_t buf[BUFSIZE];
		
		/* Record some data ... */
		if (pa_simple_read(s, buf, sizeof(buf), &error) < 0) {
		    fprintf(stderr, __FILE__": pa_simple_read() failed: %s\n", pa_strerror(error));
		    if (s){
		        pa_simple_free(s);
		    }
		}
		
		// Sending recorderded data to other server
		send(cli_sock , buf , sizeof(buf) , 0 ); 
    }
    
    return 0;
}