#include<pthread.h>
#define _USE_MATH_DEFINES
#include<math.h>
#include<stdio.h>
#include<stdlib.h>

#define DOUBLERAND ( -1 + (rand() / 16383.5))

void* tossIntoCircle(void*);

//argv is thread count and toss count 
int main(int argc, char* argv[]) {
    srand(time(NULL));
    long threadCount = strtol(argv[1], NULL, 10);
    long tossCount = strtol(argv[2], NULL, 10);

    if(threadCount == NULL || tossCount == NULL) {
        fprintf(stderr, "Invalid Arguments");
        return 0;
    }

    long tossesPerThread = (long) floor(tossCount/threadCount);
    
    pthread_t* threadHeads = malloc(threadCount * sizeof(pthread_t));
    for(long thread = 0; thread < threadCount; ++thread) {
        pthread_create(&threadHeads[thread], NULL, tossIntoCircle, (void*) tossesPerThread);
    }

    //Use master space to count remainder
    long tossesRemaining = tossCount - (threadCount * tossesPerThread);
    long numberInCircle = (long) tossIntoCircle((void*)tossesRemaining);

    void* circleCount;
    for(long thread = 0; thread < threadCount; ++thread) {
        pthread_join(threadHeads[thread], &circleCount);
        numberInCircle+= (long) circleCount;
    }

    double piEstimate = 4.0 * (numberInCircle / (double) tossCount);
    double piError = abs((piEstimate - M_PI) / M_PI) * 100l;
    printf("I estimate Pi to be: %f\nMy error is: %f\n", piEstimate, piError);
    
    free(threadHeads);
    return 0;
}

void* tossIntoCircle(void* assignedTosses) { 
    long requiredTosses = (long) assignedTosses;
    long countedInCircle = 0;
    for(long toss = 0; toss<requiredTosses; ++toss){
        double x = DOUBLERAND;
        double y = DOUBLERAND;

        if( (x*x + y*y) <= 1) {
            ++countedInCircle;
        }
    }

    return countedInCircle;
}