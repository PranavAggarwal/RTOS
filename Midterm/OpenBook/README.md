# Implementation
* I used threads for each co routine and created critical section in the function where the processing was happening so that only one thread could enter that section at a time.
* All the data is stored in global variables and data is collected from that

# Codes
* One code is a simple implementation of co routines which just prints a messaage send to the function
* The second code is the snakes and ladder implementation

# Performance

Run Number | Time(in microseconds)
------------ | ------------- 
1 | 5666 
2 | 5450
3 | 5832 
4 | 3739
