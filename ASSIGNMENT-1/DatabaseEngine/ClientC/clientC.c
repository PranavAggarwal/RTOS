// C Program for Message Queue (Client Process) 
#include <stdio.h>
#include <stdlib.h>
#include <error.h> 
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h> 
#include <sys/msg.h> 
#include <sys/time.h>

// Get clock cycle number
long long int get_cycles()
{
  long long int dst;
  __asm__ __volatile__ ("rdtsc" : "=A"(dst));
  return dst;
}

// structure for message queue 
struct msg_buffer { 
    long msg_type; 
    char msg_text[300]; 
}; 

int main() 
{ 
    struct msg_buffer buf;
    key_t key; 
    int msgid;
    long long int start, end;

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

    printf("Client C started\n");

    while(1) {
        // Message Type = 3: Client Sending message to server 
        // Message Type = 8: Server Sending message to client
        buf.msg_type = 3; 
        
        printf("File required: ");
        
        // Reading the file name
        if(fgets(buf.msg_text, sizeof buf.msg_text, stdin) != NULL) {
            int len = strlen(buf.msg_text);

            // ditch newline at end, if it exists 
            if (buf.msg_text[len-1] == '\n') buf.msg_text[len-1] = '\0';

            // Starting the timer
            start = get_cycles();
            
            // Sending the message
            if (msgsnd(msgid, &buf, len+1, 0) == -1)
                perror("msgsnd");

        }

        // Receiving the message
        if (msgrcv(msgid, &buf, sizeof buf.msg_text, 8, 0) == -1) 
        {
            perror("msgrcv");
            exit(1);
        }

        // Stoping the clock
        end = get_cycles();
        
        printf("Received by C: \"%s\"\nTime by C(in clock cycles): %lld\n", buf.msg_text, end - start); 
    }
    
    return 0; 
} 
