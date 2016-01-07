Message Queue is a very common used. The goal of design a good message queue is like below:
    1. Message Queue should have one input pin so that producers can put messages to the Queue
    2. The input pin of Message Queue can be called by multiple producers
    3. The output pin can also connected to multiple listener, each listener can register to its interested Message
    4. Message Queue works as first in and first out.
    5. Message Queue should be responsible to ensure each message is delivered without loss.
    6. Message Queue is just a container of message. So meesage in the queue is defined in a common way. user of the message queue should be responsible to interperate the data send by the message Queue

The usage workflow of message queue will be like below:
    1. user register message type into message queue so that this type of message can be accept by message Queue
    2. user register message listener to message queue with registered message type so that message can be handled
    3. user call input pin to feed message into Queue
    4. Message Queue call listener to process message
    5. message Queue delete message when process finish

Some Rule of Message Queue:
    1. messages in the queue will always be handled one by one following FIFO
    2. after message is pushed to queue, there is no relationship with the message producer. 
    3. after message is processed, message queue will destory the message which means if listener still need to hold the information of message for following steps, listener need to duplicate the message information.




