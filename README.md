# MultiThreaded_CPP
Multi-threaded example in C++
***
## Speed
This script is meant to showcase the ability and speed of multi-threading calculations in C++. In this example on a 8 thread CPU, the single thread takes about 5 seconds to complete finding the squares of all values from 1 to 300 million. While utilizing all of the 8 threads takes only about 0.0005 seconds.

## Ways of Speeding Up
The script utilizes a thread pool to manage and allocate tasks from a queue to each thread and will continuously give threads new tasks from that queue whenever that thread is finished.


