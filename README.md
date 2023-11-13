### Realtime chat 

This chat implements a TCP connection between a server and a client:
 - It has been used sockets to instatiate the connection;
 - It has been used multithreading to manage the application flow;

_If I don't use threads, the send process can block the receive process and vice versa._ 

 - Run programs in a different instances of linux shell to see the chat behavior
