#include<stdio.h>
#include<pthread.h>
#define _USE_MATH_DEFINES
#include<math.h>

#define f(x) (log(x))
#define MUTEX 'm'
#define SEMAPHORE 's'
#define BUSY_WAITING 'b'


void* mutexFunction(void*);
void* semaphoreFunction(void* rank);
void* busyWaitingFunction(void* rank);

long predictedArea = 0;
int sharedFlag;

int main(int argc, char** argv){
    long threadCount = strtol(argv[1], NULL, 10);
    
    if(threadCount == NULL) {
        fprintf(stderr, "Cannot read thread count, defaulting to 4");
        threadCount = 4;
    }

    void* threadFunction;
    switch(argv[2][0]) {
       //No locks selected, default to Mutex
        default:
            fprintf(stderr, "Lock mode incorrectly specified, defaulting to Mutex\n");
        case MUTEX: 
            threadFunction = &mutexFunction;
        break;
        case SEMAPHORE: 
            threadFunction = &semaphoreFunction;
        break;
        case BUSY_WAITING: 
            sharedFlag = 0;
            threadFunction = &busyWaitingFunction;
        break;
    }

    double a = 1, b = pow(M_E, 4), n = 1024;

    predictedArea = (f(a) / 2) + (f(b) /2);

    pthread_t* threadHeads = malloc(threadCount * sizeof(pthread_t));
    for(long thread = 0; thread < threadCount; ++thread) {
        pthread_create(&threadHeads[thread], NULL, threadFunction, (void*) thread);
    }
    //Master Space
    for(long thread = 0; thread < threadCount; ++thread) {
        pthread_join(threadHeads[thread], NULL);
    }

    predictedArea*= (b-a)/n;
    return 0;
}

void* mutexFunction(void* rank) {
    long localSum = 0;
    for(long i = lowBound; i < highBound; ++i) {
        localSum+= f(a + i((b-a) / n));
    }
    //TODO Update shared variable

}

void* semaphoreFunction(void* rank) {
    long localSum = 0;
    for(long i = lowBound; i < highBound; ++i) {
        localSum+= f(a + i((b-a) / n));
    }
    //TODO Update shared variable
    
}

void* busyWaitingFunction(void* rank) {
    long localSum = 0;
    for(long i = lowBound; i < highBound; ++i) {
        localSum+= f(a + i((b-a) / n));
    }
    //TODO Update shared variable
}