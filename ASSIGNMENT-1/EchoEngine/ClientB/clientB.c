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
    char msg_text; 
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

    printf("Client B started\n");

    printf("Charecter to send: ");
    while(1) {
        // Message Type = 2: Client Sending message to server 
        // Message Type = 7: Server Sending message to client
        buf.msg_type = 2; 
        
        // Scanning the character to be sent
        scanf("%c", &buf.msg_text);
        
        // If charecter in newline, changing it to NULL
        if (buf.msg_text == '\n') buf.msg_text = '\0';

        // If not a NULL charecter then sending the charecter
        if(buf.msg_text){
            start = get_cycles();
        
            // Sending the message
            if (msgsnd(msgid, &buf, 1, 0) == -1) 
                perror("msgsnd");

            // Receiving the response from server
            if (msgrcv(msgid, &buf, sizeof buf.msg_text, 7, 0) == -1) 
            {
                perror("msgrcv");
                exit(1);
            }
            end = get_cycles();
            // Printing the response
            printf("Received by B: \"%c\"\nTime by B(in clock cycles): %lld\n", buf.msg_text, end - start); 
        }
        else{
            // If null charecter asking user if he/she wants to send any other charecter
            printf("Charecter to send:");    
        }
        // count++;
    }
    
    return 0; 
} 
