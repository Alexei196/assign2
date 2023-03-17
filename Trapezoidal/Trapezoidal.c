#include<stdio.h>
#include<stdlib.h>
#define _USE_MATH_DEFINES
#include<math.h>
#include<semaphore.h>
#include<sys/resource.h>
#include<pthread.h>

#define MUTEX 'm'
#define SEMAPHORE 's'
#define BUSY_WAITING 'b'
#define f(x) (log(x))
double a = 1, b, n;

void* mutexFunction(void*);
void* semaphoreFunction(void* rank);
void* busyWaitingFunction(void* rank);

double sharedPredictedArea = 0;
int sharedFlag, calculationsPerThread;
sem_t sharedSemaphore;
pthread_mutex_t sharedMutex;

int main(int argc, char** argv){
    //Occurs when not enough arguments are supplied to run function
    if(argc < 3) {
        fprintf(stderr, "Usage: Trapezoidal.exe <ThreadCount> <LockType>\n");
        fprintf(stderr, "Optionally: Trapezoidal.exe <ThreadCount> <LockType> <a> <b> <n>\n");
        fprintf(stderr, "ThreadCount: Amount of PThreads desired by user\n");
        fprintf(stderr, "LockType: type of lock to be used by program. Specify m s, or b for Mutex, Semaphore, or Busy-Waiting respectively\n");
        fprintf(stderr, "a: replacement for a in Trapezoidal function\n");
        fprintf(stderr, "b: replacement for b in Trapezoidal function\n");
        fprintf(stderr, "n: replacement for n in Trapezoidal function\n");
        return 1;
    }
    if(argc > 5) {
        a = strtod(argv[3], NULL);
        b = strtod(argv[4], NULL);
        n = strtod(argv[5], NULL);
        if(a == 0 || b == 0 || n == 0){
            fprintf(stderr, "Error reading arguments! They cannot be 0!\n");
            return 1;
        }
    } else {
        a = 1.0, b = pow(M_E, 4.0), n = 1024.0;
    }
    //Receive and verify command line inputs
    long threadCount = strtol(argv[1], NULL, 10);
    char lockMode = argv[2][0];
    
    //Should the arguments not be allocated, exit program
    if(threadCount == 0 || lockMode == 0) {
        fprintf(stderr, "Error reading arguments! They cannot be zero!\n");
        return 1;
    }
    //Decide on the function run by the threads
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
    calculationsPerThread = (int) floor( n / (double) threadCount);
    int furtherCalculationsRequired = n-1 - calculationsPerThread * threadCount;
    //Create and give the range to each pthread 
    pthread_t* threadHeads = malloc(threadCount * sizeof(pthread_t));
    long startNumber;
    for(long thread = 0; thread < threadCount; ++thread) {
        startNumber = thread * calculationsPerThread;
        pthread_create(&threadHeads[thread], NULL, threadFunction, (void*) startNumber);
    }
    //Master Space used for remainder calculations and the leftover values not yet added
    double masterSum = (f(a) / 2.0) + (f(b) / 2.0);
    for(long i = n-1 -furtherCalculationsRequired; i < n; ++i){
        masterSum+= f(a + (double)i*((b-a) / n));
    }
    //Join all Pthreads
    for(long thread = 0; thread < threadCount; ++thread) {
        pthread_join(threadHeads[thread], NULL);
    }
    //Final Calculation after all threads have exited
    sharedPredictedArea = (sharedPredictedArea+masterSum) * ((b-a)/n);
    //Report Results
    struct rusage resourceMeasurer;
    getrusage(RUSAGE_SELF, &resourceMeasurer);
    printf("I predict that the curve has an area of %f\n", sharedPredictedArea);
    printf("My System time is %ld.%06ld\n", resourceMeasurer.ru_stime.tv_sec, resourceMeasurer.ru_stime.tv_usec);
    printf("My User time is %ld.%06ld\n", resourceMeasurer.ru_utime.tv_sec, resourceMeasurer.ru_utime.tv_usec);
    //Allocated resources must be destroyed according to mode
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
    double localSum = 0;
    long lowBound = (long) boundStart;
    long highBound = lowBound+calculationsPerThread;
    for(long i = lowBound; i < highBound; ++i) {
        localSum+= f(a + (double)i*((b-a) / n));
    }
    //Update shared variable
    pthread_mutex_lock(&sharedMutex);
    sharedPredictedArea += localSum;
    pthread_mutex_unlock(&sharedMutex);
    return NULL;
}

void* semaphoreFunction(void* boundStart) {
    double localSum = 0;
    long lowBound = (long) boundStart;
    long highBound = lowBound+calculationsPerThread;
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
    double localSum = 0;
    long lowBound = (long) boundStart;
    long highBound = lowBound+calculationsPerThread;
    long rank = lowBound / calculationsPerThread;
    for(long i = lowBound; i < highBound; ++i) {
        localSum+= f(a + i*((b-a) / n));
    }
    //Update shared variable
    //Busy waiting has each thread wait until its rank appears on the shared flag
    while(sharedFlag != rank);
    sharedPredictedArea+= localSum;
    sharedFlag += 1;
    return NULL;
}