// Program to demonstrate POSIX threads and pthread_mutex synchronization
// Estimates PI through monte carlo simulation
// Supply thread number in argument; more threads -> better estimation
// Author: Adwait Wadekar

#include <sys/types.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <math.h>
#include <time.h>

typedef struct{
  int arr_len;
} args_struct;

double g_PI = 0.0;

// Global Mutex Variable
pthread_mutex_t mtx;

void* monte_carlo(void* v_as) {

    args_struct* as = v_as;
    int arr_len     = (*as).arr_len;
    
    for (int i = 0; i < arr_len; i++) {
    
        double random_x = rand()/(double)RAND_MAX;
        double random_y = rand()/(double)RAND_MAX;

	// Lock mutex
	pthread_mutex_lock(&mtx);	

	// Protect g_PI in crit. section
        g_PI += random_x*random_x + random_y*random_y <= 1;

	// Unlock mutex
	pthread_mutex_unlock(&mtx);
    }
    pthread_exit(0);
}

int main(int argc, char* argv[])
{

    int i;
    void* v_args;

    args_struct* args = malloc(sizeof(args_struct));
    double pi = 0;
    int num_threads = 50;
    
    pthread_t thread[num_threads];
    srand(time(NULL));
    
    // Initialize mutex
    pthread_mutex_init(&mtx, NULL);

    if (argc != 2) {
        printf("ERR: Only 1 argument needed\n");
        exit(1);
    }
    
    int len = atoi(argv[1]);
    (*args).arr_len = len;
    v_args = (void*)(args);

    // Thread handling
    for (i = 0; i < num_threads; i++) {
        if (pthread_create(&thread[i], NULL, &monte_carlo, v_args) != 0) {
            printf("ERR: Monte Carlo thread created incorrectly\n");
            // Join waits for exit() to be called
            exit(0); 
        }
    }
    
    for (i = 0; i < num_threads; i++)
        pthread_join(thread[i], NULL);

    pi = g_PI*4.0/(len * num_threads);

    printf("Pi is: %f\n", pi);
    free(args);
    return 0;
}

