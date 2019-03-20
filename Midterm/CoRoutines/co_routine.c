#include <stdio.h> 
#include <string.h> 
#include <pthread.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <time.h> 

//Global  Variables
#define length 100 							//Maximum number of coroutine
pthread_t thread_id[length]; 				//Array of all threads
int thread_count = 0;						
pthread_mutex_t thread_locks[length]; 		//Array of all mutex locks for each thread
pthread_mutex_t lock;						//Central lock for the program
char message[length] = "\0";				//Containing data of all variables 

// Initializing all mutex
void cr_init(){	
	int i = 0; 
	for(i = 0; i < 100; i++){
		pthread_mutex_init(&(thread_locks[i]), NULL);
		pthread_mutex_lock(&(thread_locks[i]));
	}

	pthread_mutex_lock(&lock);
}

// Create a co-routine by passing function pointer
int cr_create(void *(*f)(void *)){
	pthread_create(&(thread_id[thread_count++]), NULL, f, NULL);
	pthread_mutex_lock(&lock);
	return thread_count - 1; 
}

// Send data to a co routine and continue execution(handle.send function)
void cr_send(int cr_id, char * msg){
	strcpy(message, msg);
	pthread_mutex_unlock(&(thread_locks[cr_id]));
	pthread_mutex_lock(&lock);
}

// Breakpoints within functions (Yeild in python)
void cr_yeild(int cr_id){
	pthread_mutex_unlock(&lock);
	pthread_mutex_lock(&(thread_locks[cr_id]));
}

// Killing a co routine (closing the handle in python)
void cr_kill(int cr_id){
	pthread_cancel(thread_id[cr_id]);
	pthread_mutex_destroy(&(thread_locks[cr_id])); 
}

// Co Routine function
void* print_mesage(void *arg) 
{ 
	int cr_id = thread_count - 1;
	
	while(1){
		cr_yeild(cr_id);
		printf("Message: %s\n", message);  
	}

	return NULL; 
} 

int main(void) 
{ 	
	// Init
	cr_init();

	// Create some co routines 
	int cr1 = cr_create(&print_mesage);
	
	// Sending messages to co routines
	cr_send(cr1, "Hi This is Pranav");
	cr_send(cr1, "This is a simple CoRoutine Implementaion");
	
	// Killing the routine
	cr_kill(cr1);

	return 0; 
} 
