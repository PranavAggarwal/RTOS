# Implementation
* I used threads for each co routine and created a mutex lock for each thread and a main lock for critical section so that at once only one thread could play.
* The thread locks are there so that the code would pause where the yeild command is present in the function and will resume only once the a message has been sent by the thread.
* For data communication between threads, the data has been stored in global variables

# Codes
* One code is a simple implementation of co routines which just prints a messaage send to the function
* The second code is the snakes and ladder implementation

# Performance

The below performance is for the snakes and ladder game

Run Number | Time(in microseconds)
------------ | ------------- 
1 | 7170 
2 | 6169
3 | 5971 
4 | 5336
5 | 5100
6 | 6722
7 | 6047
8 | 6432 
9 | 6090
10 | 5619
