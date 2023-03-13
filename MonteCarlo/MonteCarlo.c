#include<pthread.h>
#define _USE_MATH_DEFINES
#include<math.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/time.h>

#define DOUBLERAND ( -1 + (rand() / 16383.5))

void* tossIntoCircle(void*);

//argv is thread count and toss count 
int main(int argc, char* argv[]) {
    struct timeval startTime, endTime;
    gettimeofday(&startTime, NULL);

    srand(time(NULL));
    long threadCount = strtol(argv[1], NULL, 10);
    long long int tossCount = (long long int) strtol(argv[2], NULL, 10);

    if(threadCount == NULL || tossCount == NULL) {
        fprintf(stderr, "Invalid Arguments");
        return 0;
    }

    long long int tossesPerThread = (long long int) floor( (double) tossCount/threadCount);
    
    pthread_t* threadHeads = malloc(threadCount * sizeof(pthread_t));
    for(long thread = 0; thread < threadCount; ++thread) {
        pthread_create(&threadHeads[thread], NULL, tossIntoCircle, (void*) tossesPerThread);
    }

    //Use master space to count remainder
    long long int tossesRemaining = tossCount - (threadCount * tossesPerThread);
    long long int numberInCircle = (long long int) tossIntoCircle((void*)tossesRemaining);

    void* circleCount;
    for(long thread = 0; thread < threadCount; ++thread) {
        pthread_join(threadHeads[thread], &circleCount);
        numberInCircle+= (long long int) circleCount;
    }

    double piEstimate = 4.0l * (numberInCircle / (double) tossCount);
    double piError = abs((piEstimate - M_PI) / M_PI) * 100l;
    gettimeofday(&endTime, NULL);
    long totalTime = (endTime.tv_sec * 1000000l + endTime.tv_usec)- (startTime.tv_sec * 1000000l + startTime.tv_usec));
    printf("I estimate Pi to be: %f\nMy error is: %f\n", piEstimate, piError);
    printf("I took %ld seconds to complete this process\n", totalTime);
    free(threadHeads);
    return 0;
}

void* tossIntoCircle(void* assignedTosses) { 
    long long int requiredTosses = (long long int) assignedTosses;
    long long int countedInCircle = 0;
    for(long toss = 0; toss<requiredTosses; ++toss){
        double x = DOUBLERAND;
        double y = DOUBLERAND;

        if( (x*x + y*y) <= 1) {
            ++countedInCircle;
        }
    }

    return countedInCircle;
}