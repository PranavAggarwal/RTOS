## Assignment
* Implementing a IP Phone where one can hear both sides clearly

## Running the code
* In each system run the server and client code parralelly since server is receiving the strem and client is sending the stream.
* While executing the client, give the server IP as the first argument after the client executable(i.e., argv[1])

## Experience
* When I just executed the recording code given in PulseAudio documentation, I got some giberish on the STDOUT and nothing was getting saved.
  * To solve this I just stored the data in one file.
* While sendong data from client, the received voice was not clear and the speed of the voice was very fast, even some parts were getting cut.
  * To solve this I reduced the sampling rate and it worked
  
* We could hear some noise in the background throughout the time which I was not able to fix.

## Improvements to make it better
* I think the above problem could be solved by using some filter to remove the noise.
* We could encrypt the data, so that no one in the moddle could hear the talk.
