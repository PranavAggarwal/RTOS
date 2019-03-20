#include<stdio.h> 
#include<string.h> 
#include<pthread.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<time.h> 
#include <sys/time.h>

// Global Variables
#define length 5							// Number of players
pthread_t tid[length];                		// Thread for each player
long int tcnt = 0;
pthread_mutex_t tlocks[length]; 			// Creating lock for each thread
pthread_mutex_t lock;						// Main lock for critical section
char mesg[length] = "\0";					// Message to be sent

// Initializing all mutex
void cr_init(){
	int i = 0; 
	for(i = 0; i < length; i++){
		pthread_mutex_init(&(tlocks[i]), NULL);
		pthread_mutex_lock(&(tlocks[i]));
	}

	pthread_mutex_lock(&lock);
}

// Create a co-routine by passing function pointer
int cr_create(void *(*f)(void *)){
	pthread_create(&(tid[tcnt++]), NULL, f, NULL);
	pthread_mutex_lock(&lock);
	return tcnt - 1;
}

// Send data to a co routine and continue execution(handle.send function)
void cr_send(int cr_id, char * msg){
	strcpy(mesg, msg);
	pthread_mutex_unlock(&(tlocks[cr_id]));
	pthread_mutex_lock(&lock);
}

// Breakpoints within functions (Yeild in python)
void cr_yeild(int cr_id){
	pthread_mutex_unlock(&lock);
	pthread_mutex_lock(&(tlocks[cr_id]));
}

// Killing a co routine (closing the handle in python)
void cr_kill(int cr_id){
	pthread_cancel(tid[cr_id]);
	pthread_mutex_destroy(&(tlocks[cr_id])); 
}

// Data required for snakes and ladders
int p_location[length];										// Location of each player
int snake_start[8] = {17, 54, 62, 64, 87, 92, 95, 98};    	// Start of snake
int snake_end[8] = {7, 34, 19, 60, 36, 72, 75, 79};     	// End of snake
int ladder_start[10] = {1, 4, 9, 21, 28, 51, 72, 80};		// Start of ladder
int ladder_end[10] = {38, 14, 31, 42, 84, 67, 91, 99};		// End of ladder
int p_win[length];											// Has a player won
int win[length];                                            // Who came at what position
int no_win = 0;												// Number of winners

// Rolls the die
int roll_dice(){
	
	return (rand()%6) + 1;
}

// Checks if player reached any snake or ladder
int get_index(int v, int * l, int len){
	int i = 0;
	for(i = 0; i < len; i++){
		if(l[i] == v) return i;	
	}
	return -1;
}

// Co ROutine for game
void* game(void *arg) 
{ 
	int cr_id = tcnt - 1;
	p_location[cr_id] = 0;
	p_win[cr_id] = 0;
	
	while(1){
		cr_yeild(cr_id);
		printf("Player Playing: %d Position: %d\n", cr_id, p_location[cr_id]);
		int die_roll = roll_dice();
		printf("\tDice value %d\n", die_roll);

		// If dice rolls a value such that player is at position greater than 100, do nothing
		if(p_location[cr_id] + die_roll <= 100){
			p_location[cr_id] += die_roll;
		}

		while(1){
			// If did not reachplayer reached snake or ladder
			if(get_index(p_location[cr_id], snake_start, 10) == -1 && get_index(p_location[cr_id], ladder_start, 10) == -1){
				printf("\tPlayer at %d\n", p_location[cr_id]);
				break;
			}
			// If reached a snake
			else if(get_index(p_location[cr_id], snake_start, 10) != -1){
				int i = get_index(p_location[cr_id], snake_start, 10);
				p_location[cr_id] = snake_end[i];
				printf("\tPlayer bit by a snake and is now at %d\n", p_location[cr_id]);
			}
			// If reached a ladder
			else if(get_index(p_location[cr_id], ladder_start, 10) != -1){
				int i = get_index(p_location[cr_id], ladder_start, 10);
				p_location[cr_id] = ladder_end[i];	
				printf("\tPlayer climbed a ladder and is now at %d\n", p_location[cr_id]);
			}	
		}
		
		// Check for win
		if(p_location[cr_id] == 100){
			printf("Player %d wins!\n", cr_id);
			win[no_win] = cr_id;
			no_win += 1;
			printf("%d\n", no_win);
			p_win[cr_id] = 1;
			while(1){
				cr_yeild(cr_id);
			}
		}
	
	}

	return NULL; 
} 

// Main function
int main(void) 
{ 	
	srand(time(0));
	struct timeval stop, start;
	cr_init();
	gettimeofday(&start, NULL);

	// Create some co routines 
	int player1 = cr_create(&game);
	int player2 = cr_create(&game);
	int player3 = cr_create(&game);
	int player4 = cr_create(&game);
	printf("Before Seg\n");
	// Play till n-1 player has not won
	while(p_win[player1] == 0 | p_win[player2] == 0 | p_win[player3] == 0 | p_win[player4] == 0){
		// if(no_win != length -2){
			if(p_win[player1] == 0){
				cr_send(player1, "Play");
			}
			if(p_win[player2] == 0){
				cr_send(player2, "Play");
			}
			if(p_win[player3] == 0){
				cr_send(player3, "Play");
			}
			if(p_win[player4] == 0){
				cr_send(player4, "Play");
			}
		// }
	}

	// Finding which player came 4th
	// int i;
	// for(i = 0; i < length; i++){
	// 	if (p_win[i] == 0) {
	// 		win[3] = i;
	// 	}
	// }
	
	// Winners
	printf("WINNERS\n");
	printf("\t 1st place : player %d\n", win[0]);
	printf("\t 2nd place : player %d\n", win[1]);
	printf("\t 3rd place : player %d\n", win[2]);
	printf("\t 4th place : player %d\n", win[3]);

	// Kill the routines
	cr_kill(player1);
	cr_kill(player2);
	cr_kill(player3);
	cr_kill(player4);

	gettimeofday(&stop, NULL);
	printf("Total time taken: %lu\n", stop.tv_usec - start.tv_usec);
		
	return 0; 
} 

