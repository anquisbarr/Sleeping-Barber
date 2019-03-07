/*
 * Kristina Kolibab
 * Sleeping Barber
 * Operating Systems S2019
 * Feb. 20th, 2019
 *
 * compile: gcc sleeping_barber.c -pthread
 * execute: ./a.out
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdint.h> /* uintptr_t */
#include <inttypes.h> /* uintptr_t */

int NUM_BARBERS	= 3;
int NUM_CHAIRS = 10;
int NUM_CUSTOMERS = 25;
int CUSTOMER_MAX_INTERVAL = 2;
int HAIRCUT_TIME = 4; /* 4 secs, I changed this var for testing purposes */

extern int errno;

/* My mutex lock will be used only once over the critical section for 
 * decrementing my NUM_CHAIRS var. I do not need it later for when I
 * increment the same var, because at that point my customers will be
 * waiting for one of the three barber semaphores. */
pthread_mutex_t lock;
/* My barber semaphores will be used to take customers one at a time
 * to receive their haircut, this is enforced through sem_wait and
 * sem_post, where I also increment the NUM_CHAIRS var. */
sem_t barber;

/* Sleeping Barber function */
void* get_haircut(void* args)
{
	/* Cast void pointer to signed long int */
	int i = (intptr_t)args;

	/* Begin actual work */
	while(NUM_CHAIRS == 0){
		printf("Customer %d is waiting\n", i);
		sleep(5);
	}  
	if(NUM_CHAIRS >= 1 && NUM_CHAIRS <= 10) {		
		/* Wait for lock before entering critical section, once inside,
 		 * decrement NUM_CHAIRS and print out such. Afterwards I can
 		 * simply unlock the mutex. */
		pthread_mutex_lock(&lock);
		NUM_CHAIRS--;
		printf("Customer %d entered waiting room, %d chairs available\n", i, NUM_CHAIRS);
		pthread_mutex_unlock(&lock);

		/* Wait for available barber, then increment NUM_CHAIRS and
		 * print out that I've done so */
		sem_wait(&barber);
		NUM_CHAIRS++;
		printf("Customer %d went with barber, %d chairs available\n", i, NUM_CHAIRS);

		/* Recieve haircut and signal that a barber is available */
		sleep(HAIRCUT_TIME);
		printf("Customer %d recieved haircut\n", i);
		sem_post(&barber);
	}
	pthread_exit(NULL);
	return NULL;
}

/* Driver code */
int main(int argc, char **argv)
{
	/* Declare data and threads */
	int iteration, sleep_time, result;
	pthread_t threads[NUM_CUSTOMERS];

	/* Initialize mutex and semaphores */
    pthread_mutex_init(&lock, NULL);
	sem_init(&barber, 0, NUM_BARBERS);
	
	if (argc > 1){
		NUM_BARBERS = atoi(argv[1]);
		if (NUM_BARBERS < 1){
			printf("There must be at least one barber\n");
			return(1);
		}
	}
	if (argc > 2){
		NUM_CHAIRS = atoi(argv[2]);
		if (NUM_CHAIRS < 1){
			printf("There must be at least one chair\n");
			return(1);
		}
	}
	if (argc > 3){
		NUM_CUSTOMERS = atoi(argv[3]);
		if (NUM_CUSTOMERS < 1){
			printf("There must be at least one customer\n");
			return(1);
		}
	}
	if (argc > 4){
		CUSTOMER_MAX_INTERVAL = atoi(argv[4]);
		if (CUSTOMER_MAX_INTERVAL < 0){
			printf("The customers cannot come in negative time\n");
			return(1);
		}
	}
	if (argc > 5){
		HAIRCUT_TIME = atoi(argv[5]);
		if (HAIRCUT_TIME < 1){
			printf("Haircuts must take at least one second\n");
			return(1);
		}
	}

	printf("Welcome to the barbershop problem!\n");
	printf("For this run, we have:\n");
	printf("%i Barbers\n", NUM_BARBERS);
	printf("%i Chairs\n", NUM_CHAIRS);
	printf("%i customers will come in with delay from 0 to %i between them.\n", NUM_CUSTOMERS, CUSTOMER_MAX_INTERVAL);
	printf("A haircut takes %i seconds.\n\n", HAIRCUT_TIME);
	printf("Beginning Simulation.\n\n");

	/* Create threads */
	for(iteration=0; iteration<NUM_CUSTOMERS; ++iteration){
		pthread_create(&threads[iteration], NULL, get_haircut, (void*)(intptr_t)(iteration+1));
		sleep((rand() % 2)+1);
	}

	/* Join threads */
	for(iteration=0; iteration<NUM_CUSTOMERS; ++iteration){
		pthread_join(threads[iteration], NULL);		
	}

	/* Clean up */
	pthread_mutex_destroy(&lock);
	sem_destroy(&barber);
	printf("\nBarbershop Problem Completed\n");
	
	return(0);
}

