#include<pthread.h>
#define _USE_MATH_DEFINES
#include<math.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/time.h>

#define DOUBLERAND ( -1 + ( (double)rand() / (RAND_MAX / 2)))

void* tossIntoCircle(void*);

pthread_mutex_t numberInCircleMutex;

long long int numberInCircle = 0;

//argv is thread count and toss count 
int main(int argc, char* argv[]) {
    struct timeval startTime, endTime;
    gettimeofday(&startTime, NULL);
    //Count and verify argument variables 
    long threadCount = strtol(argv[1], NULL, 10);
    long long int tossCount = (long long int) strtol(argv[2], NULL, 10);

    if(threadCount == NULL || tossCount == NULL) {
        fprintf(stderr, "Invalid Arguments");
        return 1;
    }
    if(threadCount < 1 || tossCount < 1) {
        fprintf(stderr, "Arguments cannot be less than 1!");
        return 1;
    }
    if(threadCount > tossCount) {
        fprintf(stderr, "There are more threads than tosses!");
        return 1;
    }

    srand(time(NULL));
    long long int tossesPerThread = (long long int) floor( (double) (tossCount/threadCount));    
    pthread_t* threadHeads = malloc(threadCount * sizeof(pthread_t));
    pthread_mutex_init(&numberInCircleMutex, NULL);

    for(long thread = 0; thread < threadCount; ++thread) {
        pthread_create(&threadHeads[thread], NULL, tossIntoCircle, (void*) tossesPerThread);
    }

    //Use master space to count all tosses not partitioned into threads  
    long long int tossesRemaining = tossCount - (threadCount * tossesPerThread);
    tossIntoCircle((void*)tossesRemaining);

    for(long thread = 0; thread < threadCount; ++thread) {
        pthread_join(threadHeads[thread], NULL);
    }
    //Results Reporting
    printf("Circle Number: %lld, Toss Count: %lld\n",numberInCircle, tossCount);
    double piEstimate = 4.0l * (numberInCircle / (double) tossCount);
    double piError = fabs((piEstimate - M_PI) / M_PI) * 100l;
    gettimeofday(&endTime, NULL);
    long totalTime = (endTime.tv_sec * 1000000l + endTime.tv_usec)- (startTime.tv_sec * 1000000l + startTime.tv_usec);
    printf("I estimate Pi to be: %f\nMy error is about: %.2f%%\n", piEstimate, piError);
    printf("I took %ld seconds to complete this process\n", totalTime);
    //Resource Destruction
    free(threadHeads);
    pthread_mutex_destroy(&numberInCircleMutex);
    return 0;
}

void* tossIntoCircle(void* assignedTosses) { 
    long long int requiredTosses = (long long int) assignedTosses;
    long long int countedInCircle = 0;
    for(long toss = 0; toss<requiredTosses; ++toss){
        double x = DOUBLERAND;
        double y = DOUBLERAND;
        if( (x*x + y*y) <= 1) {
            countedInCircle+=1;
        }
    }
    //Mutex protected update to number in circle
    pthread_mutex_lock(&numberInCircleMutex);
    numberInCircle = numberInCircle + countedInCircle;
    pthread_mutex_unlock(&numberInCircleMutex);
    return NULL;
}