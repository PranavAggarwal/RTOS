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
struct msg_buffer server_func(char filename[], int client_no){
    FILE *f;

    // defining a message buffer
    struct msg_buffer buf;
    buf.msg_type = 5 + client_no;
    
    // Check for invalid instruction
    char comp[length] = "GET ";
    char instruction[length];
    strncpy(instruction, filename, 4);

    if(strcmp(instruction, comp) != 0){
        char str[length] = "Invalid Instruction";
        strcpy(buf.msg_text, str);
        return buf;
    }
    
    // Extracting the filename
    int len = strlen(filename);
    int i;
    for(i = 4; i <= len; i++){
        filename[i-4] = filename[i];
    }
    printf("Filename is: %s\n", filename);

    // char filenam[length] = "demofile1.txt";
    f = fopen(filename,"r");
    if(f == NULL){
        char str[length] = "Data not found";
        strcpy(buf.msg_text, str);
        return buf;
    }
    
    // Storing data in a string
    char final[length] = "", str[length];
    while(fgets(str, length, f) != NULL){
        strcat(final, str);
    }   

    fclose(f);
    // char str[length] = "Data not found ksdhg";
    strcpy(buf.msg_text, final);
    return buf;
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
            printf("Received: \"%s\"\n", buf.msg_text);
        }
        
        // Depending on the client, message is sent to the same client back          
        if(buf.msg_type == 1){   
            // Reading the file
            buf = server_func(buf.msg_text,1);
        }
        else if(buf.msg_type == 2){
            // Reading the file
            buf = server_func(buf.msg_text,2);    
        }
        else if(buf.msg_type == 3){
            // Reading the file
            buf = server_func(buf.msg_text,3);    
        }
        else if(buf.msg_type == 4){
            // Reading the file
            buf = server_func(buf.msg_text,4);    
        }
        else if(buf.msg_type == 5){
            // Reading the file
            buf = server_func(buf.msg_text,5);    
        }        
        
        // If charecter in newline, changing it to NULL
        int len = strlen(buf.msg_text);
        if (buf.msg_text[len - 1] == '\n') buf.msg_text[len - 1] = '\0';

        // Sending the message back to client
        if (msgsnd(msgid, &buf, len + 1, 0) == -1) 
            perror("msgsnd");    
    }

    return 0;
}
