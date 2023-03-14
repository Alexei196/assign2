#include<pthread.h>
#define _USE_MATH_DEFINES
#include<math.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/resource.h>

#define DOUBLERAND ( -1 + ( (double)rand() / (RAND_MAX / 2)))

void* tossIntoCircle(void*);

pthread_mutex_t numberInCircleMutex;

long long int numberInCircle = 0;

//argv is thread count and toss count 
int main(int argc, char* argv[]) {
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

    //Use master space to count all tosses not put inside threads  
    long long int tossesRemaining = tossCount - (threadCount * tossesPerThread);
    tossIntoCircle((void*)tossesRemaining);

    for(long thread = 0; thread < threadCount; ++thread) {
        pthread_join(threadHeads[thread], NULL);
    }
    
    double piEstimate = 4.0l * (numberInCircle / (double) tossCount);
    //Report results
    struct rusage resourceMeasurer;
    getrusage(RUSAGE_SELF, &resourceMeasurer);
    double piError = fabs((piEstimate - M_PI) / M_PI) * 100l;
    printf("Circle Number: %lld, Toss Count: %lld\n",numberInCircle, tossCount);
    printf("I estimate Pi to be: %f\nMy error is about: %.2f%%\n", piEstimate, piError);
    printf("My System time is %ld.%06ld\n", resourceMeasurer.ru_stime.tv_sec, resourceMeasurer.ru_stime.tv_usec);
    printf("My User time is %ld.%06ld\n", resourceMeasurer.ru_utime.tv_sec, resourceMeasurer.ru_utime.tv_usec);
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