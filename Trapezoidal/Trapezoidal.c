#include<stdio.h>
#include<pthread.h>
#define _USE_MATH_DEFINES
#include<math.h>
#include<sys/semaphore.h>

#define MUTEX 'm'
#define SEMAPHORE 's'
#define BUSY_WAITING 'b'
#define f(x) log(x)
double a = 1, b, n;

void* mutexFunction(void*);
void* semaphoreFunction(void* rank);
void* busyWaitingFunction(void* rank);

long long int sharedPredictedArea = 0;
int sharedFlag, calculationPerThread;
sem_t sharedSemaphore;
pthread_mutex_t sharedMutex;

int main(int argc, char** argv){
    //Receive and verify command line inputs
    long threadCount = strtol(argv[1], NULL, 10);
    char lockMode = argv[2][0];
    a = 1.0, b = pow(M_E, 4.0), n = 1024.0;
    if(threadCount == NULL || lockMode == NULL) {
        fprintf(stderr, "Cannot read arguments!");
        return 1;
    }

    void* threadFunction;
    switch(lockMode) {
       //No locks selected, default to Mutex
        default:
            fprintf(stderr, "Lock mode incorrectly specified, defaulting to Mutex\n");
            lockMode = MUTEX;
        case MUTEX: 
            pthread_mutex_init(&sharedMutex, NULL);
            threadFunction = &mutexFunction;
        break;
        case SEMAPHORE: 
            sem_init(&sharedSemaphore, 0, 1);
            threadFunction = &semaphoreFunction;
        break;
        case BUSY_WAITING: 
            sharedFlag = 0;
            threadFunction = &busyWaitingFunction;
        break;
    }
    //Calculate overhead for threads
    calculationPerThread = (int) floor((double) n / threadCount);

    pthread_t* threadHeads = malloc(threadCount * sizeof(pthread_t));
    int startNumber;
    for(long thread = 0; thread < threadCount; ++thread) {
        startNumber = thread * calculationPerThread;
        pthread_create(&threadHeads[thread], NULL, threadFunction, (void*) startNumber);
    }
    
    //Master Space
    for(long thread = 0; thread < threadCount; ++thread) {
        pthread_join(threadHeads[thread], NULL);
    }

    sharedPredictedArea = (sharedPredictedArea + (f(a) / 2) + (f(b) / 2)) * ((b-a)/n);
    //Report Results
    struct rusage resourceMeasurer;
    getrusage(RUSAGE_SELF, &resourceMeasurer);
    printf("I predict that the curve as an area of %lld\n", sharedPredictedArea);
    printf("My System time is %ld.%06ld\n", resourceMeasurer.ru_stime.tv_sec, resourceMeasurer.ru_stime.tv_usec);
    printf("My User time is %ld.%06ld\n", resourceMeasurer.ru_utime.tv_sec, resourceMeasurer.ru_utime.tv_usec);

    switch(lockMode) {
        case MUTEX: 
            pthread_mutex_destroy(&sharedMutex);
        break;
        case SEMAPHORE: 
            sem_destroy(&sharedSemaphore);
        break;
    }

    free(threadHeads);
    return 0;
}

void* mutexFunction(void* boundStart) {
    long localSum = 0;
    long lowBound = (int) boundStart;
    long highBound = lowBound+calculationPerThread;
    for(long i = lowBound; i < highBound; ++i) {
        localSum+= f(a + i*((b-a) / n));
    }
    //Update shared variable
    pthread_mutex_lock(&sharedMutex);
    sharedPredictedArea += localSum;
    pthread_mutex_unlock(&sharedMutex);
    return NULL;
}

void* semaphoreFunction(void* boundStart) {
    long localSum = 0;
    long lowBound = (int) boundStart;
    long highBound = lowBound+calculationPerThread;
    for(long i = lowBound; i < highBound; ++i) {
        localSum+= f(a + i*((b-a) / n));
    }
    //Update shared variable
    sem_wait(&sharedSemaphore);
    sharedPredictedArea += localSum;
    sem_post(&sharedSemaphore);
    return NULL;
}

void* busyWaitingFunction(void* boundStart) {
    long localSum = 0;
    long lowBound = (int) boundStart;
    long highBound = lowBound+calculationPerThread;
    for(long i = lowBound; i < highBound; ++i) {
        localSum+= f(a + i*((b-a) / n));
    }
    //Update shared variable
    while(sharedFlag != 0);
    sharedFlag = 1;
    sharedPredictedArea+= localSum;
    sharedFlag = 0;
    return NULL;
}