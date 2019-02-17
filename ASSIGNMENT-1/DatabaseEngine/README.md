### How To Run:

1. First execute the server and than any other client, becuase server is creating the message queue.
2. Run msg_del_q.c to delete the message queue.

### Performance Data

Number of Clients running | Time (in clock cycles)
------------ | -------------
1 | 48096
2 | 58745
3 | 63684
4 | 99009
5 | 126632

#### How was data measured?

1. All programs were running parallely and each program was sending a command in an infinite loop.
2. One by one each program was stopped to decrease the number of parallel processes.
3. The recorder clock cycles of the stopped client was observed and the a number was picked in from the range in which most clock cycles appeared.

The code uploaded does not do the same. It ask the user to give some input that should be set to the server.
