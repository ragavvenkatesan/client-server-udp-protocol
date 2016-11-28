
A port is something that contains N messages, make N=10. There would be semaphores associated with 
the port, for controlling synchronization. The port “contains” messages hence a port may be a 
pointer to a Q of messages, or an array of messages.

Declare a message type. For simplicity, a message is an array of 10 integers.
Declare a set (array of ports). The ports are numbered 0 to 99.

Routines:

Send (with appropriate arguments). The send is a “blocking” asynchronous send that is: it places 
the message in the port and returns but blocks if port is full.
 
Receive (with appropriate arguments). The receive is a “blocking” receive.
 
Message passing system by writing one (or more) servers and having several clients talk to the 
servers. Clients choose a server at random and sends a newly created message to its port. The 
Server reads from its port and recieves. No ACK sent. 

This is essentially an UDP protocol.

License is MIT, but please don't copy code if you are also doing the same project. This was a lot of 
fun to implement and I don't want you to miss out on the fun if Prof. Dasgupta assigns the same
project to you.
