// C Program for Message Queue (Client Process) 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

// structure for message queue 
struct msg_buffer { 
    long msg_type; 
    char msg_text; 
};

// Server function
char server_func(char c){
    int ascii = c;

    //  Converting upper case charecter to lower case charecter
    if((ascii >= 65) && (ascii <= 90)){
        ascii += 32;
        return ascii;
    }

    //  Converting lower case charecter to upper case charecter
    if((ascii >= 97) && (ascii <= 122)){
        ascii -= 32;
        return ascii;
    }

    return c;
}

int main()
{
    struct msg_buffer buf;
    int msgid;
    key_t key;

    // ftok to generate unique key 
    if((key = ftok("server", 'B')) == -1){
        perror("ftok");
        exit(1);
    } 

    // msgget creates a message queue and returns identifier
    if ((msgid = msgget(key, 0644 | IPC_CREAT)) == -1) {
        perror("msgget");
        exit(1);
    }
    
    printf("Server started\n");

    while(1) { 
        // Message Type = 1: ClientA Sending message to server
        // Message Type = 2: ClientB Sending message to server
        // Message Type = 3: ClientC Sending message to server
        // Message Type = 4: ClientD Sending message to server
        // Message Type = 1: ClientE Sending message to server 
        // Message Type = 6: Server Sending message to clientA
        // Message Type = 7: Server Sending message to clientB
        // Message Type = 8: Server Sending message to clientC
        // Message Type = 9: Server Sending message to clientD
        // Message Type = 10: Server Sending message to clientE
        
        // Receiving messages from the client
        if (msgrcv(msgid, &buf, sizeof buf.msg_text, 0, 0) == -1) 
        {
            perror("msgrcv");
            exit(1);
        }
        // Print message if received from client
        if(buf.msg_type == 1 || buf.msg_type == 2 || buf.msg_type == 3 || buf.msg_type == 4 || buf.msg_type == 5){
            printf("Received: \"%c\"\n", buf.msg_text);
        }
        
        // Changing the charecter as required
        buf.msg_text = server_func(buf.msg_text);
        // Depending on the client, defining msg_type          
        buf.msg_type += 5;
        
        // If charecter in newline, changing it to NULL
        if (buf.msg_text == '\n') buf.msg_text = '\0';

        // Sending the message back to client
        if (msgsnd(msgid, &buf, 1, 0) == -1) 
            perror("msgsnd");    
    }

    return 0;
}
