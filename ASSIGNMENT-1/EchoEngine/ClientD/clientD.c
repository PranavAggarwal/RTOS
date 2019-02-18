// C Program for Message Queue (Client Process) 
#include <stdio.h>
#include <stdlib.h>
#include <error.h> 
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h> 
#include <sys/msg.h> 
#include <sys/time.h>

#define length 300

// structure for message queue 
struct msg_buffer { 
    long msg_type; 
    char msg_text[length]; 
}; 

int main() 
{ 
    struct timeval start, end;
    struct msg_buffer buf;
    key_t key; 
    int msgid;
    
    // ftok to generate unique key 
    if((key = ftok("../Server/server", 'B')) == -1){
        perror("ftok");
        exit(1);
    } 
  
    // Connecting to the message queue created by server 
    if((msgid = msgget(key, 0644)) == -1){
        perror("msgget");
        exit(1);
    } 

    printf("Client D started\n");

    while(1) {
        // Message Type = 1: Client Sending message to server 
        // Message Type = 4: Server Sending message to client
        buf.msg_type = 1; 
        
        // Scanning the character to be sent
        printf("Charecter to send: ");
        scanf("%s", buf.msg_text);

        // Putting the Client number in message so that server could identify it
        buf.msg_text[1] = '4';
        buf.msg_text[2] = '\0';
        
        int len = strlen(buf.msg_text);
        
        // Get start time
        gettimeofday(&start, NULL);
        
        // Sending the message
        if (msgsnd(msgid, &buf, len+1, 0) == -1) 
            perror("msgsnd");

        // Receiving the response from server
        if (msgrcv(msgid, &buf, sizeof buf.msg_text, 5, 0) == -1) 
        {
            perror("msgrcv");
            exit(1);
        }
        gettimeofday(&end, NULL);
        // Printing the response
        printf("Received by D: \"%s\"\nTime by D(in microsecond): %lu\n", buf.msg_text, end.tv_usec - start.tv_usec); 
    }
    
    return 0; 
} 
