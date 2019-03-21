#include<stdio.h> 
#include<string.h> 
#include<pthread.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<time.h> 
#include <sys/time.h>

// COROUTINES HELPER FUNCTIONS AND VARIABLES

// Co Routines Variables
#define length 5											// Number of coroutines
pthread_t tid[length];                						// Thread for each player
long int tcnt = 0;
pthread_mutex_t tlocks[length]; 							// Creating lock for each thread
pthread_mutex_t lock;										// Main lock for critical section
char mesg[length] = "\0";									// Message to be sent

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
void cr_send(int cr_id){
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

// SNAKES AND LADDER VARIABLES AND FUNCTION

// Data required for snakes and ladders
int p_location[length];										// Location of each player
int snake_start[8] = {17, 54, 62, 64, 87, 92, 95, 98};    	// Start of snake
int snake_end[8] = {7, 34, 19, 60, 36, 72, 75, 79};     	// End of snake
int ladder_start[8] = {1, 4, 9, 21, 28, 51, 72, 80};		// Start of ladder
int ladder_end[8] = {38, 14, 31, 42, 84, 67, 91, 99};		// End of ladder
int p_win[length];											// Has a player won
int win[length];                                            // Who came at what position
int no_win = 0;												// Number of winners

// Rolls the die
int roll_dice(){
	return (rand()%6) + 1;
}

// Checks if player reached any snake or ladder
int isat_snake_or_ladder(int player_pos, int *snake_or_ladder, int len){
	int i = 0;
	for(i = 0; i < len; i++){
		if(snake_or_ladder[i] == player_pos) {
			return i;
		}	
	}
	return -1;
}

// Snakes and ladder game
void* game(void *arg) 
{ 
	int cr_id = tcnt - 1;									// Coroutine ID/ Player ID
	p_location[cr_id] = 0;									// Initializing location of player to 0
	p_win[cr_id] = 0;										// Initializing player has not yet won
	
	while(1){
		cr_yeild(cr_id);
		printf("Player Playing: %d Position: %d\n", cr_id, p_location[cr_id]);
		int die_roll = roll_dice();
		printf("\tDice value %d\n", die_roll);

		// If dice rolls a value such that player is at position greater than 100, do nothing
		if(p_location[cr_id] + die_roll <= 100){
			p_location[cr_id] += die_roll;
		}

		// If reached a snake
		if(isat_snake_or_ladder(p_location[cr_id], snake_start, 8) != -1){
			int i = isat_snake_or_ladder(p_location[cr_id], snake_start, 10);
			p_location[cr_id] = snake_end[i];
			printf("\tPlayer bit by a snake\n");
		}
		// If reached a ladder
		if(isat_snake_or_ladder(p_location[cr_id], ladder_start, 8) != -1){
			int i = isat_snake_or_ladder(p_location[cr_id], ladder_start, 10);
			p_location[cr_id] = ladder_end[i];	
			printf("\tPlayer climbed a ladder\n");
		}	
		
		printf("\tPlayer at %d\n", p_location[cr_id]);
		// Check for win
		if(p_location[cr_id] == 100){
			printf("Player %d wins!\n", cr_id);
			win[no_win] = cr_id;							// Storing which player won
			no_win += 1;									// Incrementing number of winners
			p_win[cr_id] = 1;								// Making that player state to be won
			// While loop so that the thread does not keep the lock on
			while(1){
				cr_yeild(cr_id);
			}
		}

		// If 3 players won finding who came last
		if(no_win == 3){
			int i;
			// Finding the 4th player
			for(i = 0; i < 4; i++){
				if (p_win[i] == 0) {
					win[3] = i;
				}
			}
			p_win[cr_id] = 1;								//Setting player to be won
			// While loop so that the thread does not keep the lock on
			while(1){
				cr_yeild(cr_id);
			}
		}
	
	}

	return NULL; 
} 

// MAIN
int main(void) 
{ 	
	srand(time(0));
	struct timeval stop, start;
	cr_init();
	gettimeofday(&start, NULL);								// Stating the program

	// Create some co routines 
	int player1 = cr_create(&game);
	int player2 = cr_create(&game);
	int player3 = cr_create(&game);
	int player4 = cr_create(&game);
	
	// Play till n-1 player has not won
	while(p_win[player1] == 0 | p_win[player2] == 0 | p_win[player3] == 0 | p_win[player4] == 0){	
			if(p_win[player1] == 0){
				// cr_send(player1, "Play");
				cr_send(player1);
			}
			if(p_win[player2] == 0){
				// cr_send(player2, "Play");
				cr_send(player2);
			}
			if(p_win[player3] == 0){
				// cr_send(player3, "Play");
				cr_send(player3);
			}
			if(p_win[player4] == 0){
				// cr_send(player4, "Play");
				cr_send(player4);
			}
	}
	
	// Winners
	printf("\nWinners are:\n");
	printf("1st place : player %d\n", win[0]);
	printf("2nd place : player %d\n", win[1]);
	printf("3rd place : player %d\n", win[2]);
	printf("4th place : player %d\n", win[3]);

	// Kill the routines
	cr_kill(player1);
	cr_kill(player2);
	cr_kill(player3);
	cr_kill(player4);

	gettimeofday(&stop, NULL);							// End time
	// Giving time taken by program
	printf("Total time taken: %lu\n", stop.tv_usec - start.tv_usec);
		
	return 0; 
} 

