### How To Run:

1. First execute the server and than any other client, becuase server is creating the message queue.
2. Run msg_del_q.c to delete the message queue.

### Performance Data

Number of Clients running | Time (in microseconds) | Time Range (in microseconds)
------------ | ------------- | -------------
1 | 7 | 1 - 21
2 | 17 | 15 - 22
3 | 22 | 14 - 27
4 | 26 | 21 - 36
5 | 32 | 21 - 42

The range mentioned above is the one occuring the most. And the value picked is the one which is occuring nearly the most. But sometimes the value reached in near 100 to 300 microsecond.

#### How was data measured?

1. All programs were running parallely and each program was sending a command in an infinite loop.
2. One by one each program was stopped to decrease the number of parallel processes.
3. The recorder clock cycles of the stopped client was observed and the a number was picked in from the range in which most clock cycles appeared.

The code uploaded does not do the same. It ask the user to give some input that should be set to the server.
