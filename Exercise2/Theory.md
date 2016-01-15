Atomic operation: Multiple operations are considered to be one, to assure that a thread is not paused in the middle of an operation.

MuTex: Mutual exclusion. A thread can only "speak" when it has the mutex. It is a way of unabeling the threads of messing up each other.

semaphore: A type of MuTex, where a variable is locked to a thread, and is not unlocked until the command is given.

critical section: A part of a program that demands mutual exclusion of access to run. 
