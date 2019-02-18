// C Program for Message Queue (Client Process) 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define length 300

// structure for message queue 
struct msg_buffer { 
    long msg_type; 
    char msg_text[length]; 
};

// Server function
char *server_func(char c[]){
    int ascii = c[0];

    //  Converting upper case charecter to lower case charecter
    if((ascii >= 65) && (ascii <= 90)){
        ascii += 32;
        c[0] = ascii;
    }

    //  Converting lower case charecter to upper case charecter
    if((ascii >= 97) && (ascii <= 122)){
        ascii -= 32;
        c[0] = ascii;
    }

    c[1] = '\0';
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
        // Message Type = 1: Client Sending message to server
        // Message Type = 2: Server Sending message to clientA
        // Message Type = 3: Server Sending message to clientB
        // Message Type = 4: Server Sending message to clientC
        // Message Type = 5: Server Sending message to clientD
        // Message Type = 6: Server Sending message to clientE
        
        // Receiving messages from the client
        if (msgrcv(msgid, &buf, sizeof buf.msg_text, 1, 0) == -1) 
        {
            perror("msgrcv");
            exit(1);
        }
        
        printf("Received: \"%c\"\n", buf.msg_text[0]);
        
        // Extracting the client number and letter
        char client_no = buf.msg_text[1];
        
        // Changing the charecter as required
        char *required = server_func(buf.msg_text); 
        strcpy(buf.msg_text, required);
        
        // Depending on the client, defining msg_type          
        if(client_no == '1'){
            buf.msg_type = 2;    
        }
        else if(client_no == '2'){
            buf.msg_type = 3;    
        }
        else if(client_no == '3'){
            buf.msg_type = 4;    
        }
        else if(client_no == '4'){
            buf.msg_type = 5;    
        }
        else if(client_no == '5'){
            buf.msg_type = 6;    
        }
        
        int len = strlen(buf.msg_text);

        // Sending the message back to client
        if (msgsnd(msgid, &buf, len + 1, 0) == -1) 
            perror("msgsnd");    
    }

    return 0;
}
