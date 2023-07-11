# emQueue with QoS policies
API functions for embedded devices that implement thread-safe priority multiple queue with  
the possibility to use the lifespan policy for an integer.
The lifespan policy allow to implement aging priority of data from lower to highter priority queues.

# how to run the code
- open a terminal and execute: \    
    ```make all``` --> to compile the project \
    ```make run``` --> to execute the output file \
    ```make clean``` --> to delete all the binary output files \
- it is possibile to set the number of writer and reader task and the number of priority queues in the PresentazioneEsame.h file \
    Note: number of writer must be greater or equal the number of reader




