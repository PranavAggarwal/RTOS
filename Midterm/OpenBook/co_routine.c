#include <stdio.h> 
#include <string.h> 
#include <pthread.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <time.h> 


// COROUTINES HELPER FUNCTIONS AND VARIABLES

// Co Routines Variables
#define length 5											// Number of coroutines
pthread_t tid[length];                						// Thread for each player
long int tcnt = 0;
pthread_mutex_t tlocks[length]; 							// Creating lock for each thread
pthread_mutex_t lock;										// Main lock for critical section
char mesg[1024] = "\0";									// Message to be sent

// Co Routine helper function

// Initializing all mutex
void cr_init(){
	int i = 0; 
	for(i = 0; i < length; i++){
		pthread_mutex_init(&(tlocks[i]), NULL);				// Initialising all the thread_locks
		pthread_mutex_lock(&(tlocks[i]));					// Locking all the threads so that game cant be played till all players are created
	}
	pthread_mutex_init(&lock, NULL);						// Initializing the critical section lock
	pthread_mutex_lock(&lock);								// Locking the critical section lock
}

// Create a co-routine by passing function pointer
int cr_create(void *(*f)(void *)){
	pthread_create(&(tid[tcnt++]), NULL, f, NULL);			// Creating each thread
	pthread_mutex_lock(&lock);								// Critical section so that the game function initializes
	return tcnt - 1;
}

// Send data to a co routine and continue execution(handle.send function)
void cr_send(int cr_id, char *msg){
	strcpy(mesg, msg);									// Copying message to global variable
	pthread_mutex_unlock(&(tlocks[cr_id]));					// Unlocking the thread so that the player could play the game
	pthread_mutex_lock(&lock);								// Critical section so that only one player can play at a time
}

// Breakpoints within functions (Yeild in python)
void cr_yeild(int cr_id){
	pthread_mutex_unlock(&lock);							// Allowing the unlocked player to play the game
	pthread_mutex_lock(&(tlocks[cr_id]));					// Locking the player so that it could play only once
}

// Killing a co routine (closing the handle in python)
void cr_kill(int cr_id){
	pthread_cancel(tid[cr_id]);								// Killing the thread
	pthread_mutex_destroy(&(tlocks[cr_id])); 				// Killing the mutex lock
}

// A simple message printing Co Routine function
void* print_mesage(void *arg) 
{ 
	int cr_id = tcnt - 1;
	
	while(1){
		cr_yeild(cr_id);
		printf("Seng\n");
		printf("Message sent by: %d and the message is: %s\n", cr_id, mesg);  
	}

	return NULL; 
} 

// MAIN
int main(void) 
{ 	
	// Init
	cr_init();

	// Create some co routines
	printf("Seg\n"); 
	int cr1 = cr_create(&print_mesage);
	int cr2 = cr_create(&print_mesage);
	
	// Sending messages to co routines
	cr_send(cr1, "Hi This is Pranav");
	cr_send(cr1, "This is a simple CoRoutine Implementaion");
	cr_send(cr2, "This is second co routinee");
	cr_send(cr2, "This is also from second coroutine");
	
	// Killing the routine
	cr_kill(cr1);
	cr_kill(cr2);

	return 0; 
} 
