Andrew Rossman
823166078

File Manifest:
MonteCarlo/MonteCarlo.c
MonteCarlo/MonteCarlo.exe
Trapezoidal/Trapezoidal.c
Trapezoidal/Trapezoidal.exe

Complilation for functions: 
gcc MonteCarlo.c -o MonteCarlo.exe -lpthread -lm
gcc Trapezoidal.c -o Trapezoidal.exe -lpthread -lm

Command line Calling:
MonteCarlo/MonteCarlo.exe <THREADCOUNT> <TOSSAMOUNT>
Thread Count: Specify Number of threads used by program
Toss Amount: Specify amount of tosses to be thrown in total 

Trapezoidal/Trapezoidal.exe <THREADCOUNT> <LOCKTYPE>
or
Trapezoidal/Trapezoidal.exe <THREADCOUNT> <LOCKTYPE> <A> <B> <N>
Thread Count: Specify Number of threads used by program
Lock Type: character of 3 options m, s, or b to specify type of lock used in program  
A: Replacement for A in the function used in Trapezoidal approximation
B: Replacement for A in the function used in Trapezoidal approximation
N: Replacement for A in the function used in Trapezoidal approximation

Answers to part 2 of question 2:
Mutex: 
Mutexes will lock the critical section causing other processes to block when they attempt to use the mutex until the locking thread releases the mutex. This method unlike others does not guarantee the order of thread execution.  

Semaphore: 
A semaphore can have a greater than one locking value in which multiple resources can be shared by the threads. Wait and post operations are as simple as increment and decrements to a shared flag, which can be initialized higher than 1 so that multiple threads can access the shared resources. However, semaphores suffer from similar issues as busy waiting in which the constant checking results in computationally expensive constant checks on the shared flag. 

Busy-Waiting: 
Busy waiting constantly checks the critical section's protection flag to see if it has changed, meaning a thread will be notified of the sections availablity as fast as it can update. It can also be used to guarantee the order of execution of threads in the program. However, because it checks the flag constantly, it is more computationally expensive than its competitors.
