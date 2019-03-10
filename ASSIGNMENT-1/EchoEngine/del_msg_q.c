// C Program for Message Queue (Client Process) 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

int main()
{
    int msgid;
    key_t key;

    // ftok to generate unique key 
    if((key = ftok("Server/server", 'B')) == -1){
        perror("ftok");
        exit(1);
    } 

    if ((msgid = msgget(key, 0644)) == -1) { /* connect to the queue */
        perror("msgget");
        exit(1);
    }

    // Removing the message queue 
    if (msgctl(msgid, IPC_RMID, NULL) == -1) 
    {
        perror("msgctl");
        exit(1);
    }


    return 0;
}
