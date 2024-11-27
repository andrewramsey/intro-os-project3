CS 3113 Intro to Operating Systems Project 3 (A shared Protected Circular Queue and Communication between threads)

This program uses a circular buffer to communicate between two threads, the producer and consumer. Producer reads characters one by one from a file (mytest.dat), and places them into buffer until end of file. Producer informs consumer when it's finished putting last char in buffer. Then the consumer thread reads the characters one by one from buffer and prints it. 

To compile:
In UNIX run the command "gcc project3.c -lpthread -lrt -o project3" to compile 
Then run "./project3" and it will run