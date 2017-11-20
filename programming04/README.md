adifalco, bdalgarn , bhoeltin

Both the server and client directories contain Makefiles necessary to compile
a chat room application. The flow of the program is described below:


1. Server opens a port, creates the TCP/UDP socket, and goes into "wait for 
connection" state, and actively listens for socket connections. 
2. Client logs into the system by connecting to the server on the appropriate port.
3. Client sends username.
4. Server checks to see if it is a new user or existing user and requests a 
   password. Note: Store users and their passwords in a file rather than in memory.
5. Client sends password.
6. Server either registers a new user or checks to see if the password matches. 
   Server then sends acknowledgement to the client. Note: multiple clients should be 
   able to register at the same time.
7. Server continues to wait for operation command from a client or a new client connection.
8. Client goes into "prompt user for operation" state and prompts user for operation.
9. Client passes operation (B: Message Broadcasting, P: Private Messaging, E: Exit) to server.
10. Operation is executed as follows:

-----------------------------------------------------------------------------------------------

B:	Client sends operation (B) to broadcast a message to all active clients 

	Server sends the acknowledgement back to the client to prompt for the message 
	to be sent.

	Client sends the broadcast message to the server.

	Server receives the message and sends that message to all other client connections. 

	Server sends confirmation that the message was sent. 

	Client receives and prints the confirmation.

	Client and server return to "prompt user for operation" and "wait for operation 
	from client" state respectively.

------------------------------------------------------------------------------------------------

P:	Client sends operation (P) to leave a message to a specific client.

	Server sends the list of current online users. We assume that any client can go 
	offline by using operation (E).

	Client receives the list of online users from the server.

	Client prompts the user for and sends the username to send a message to.

	Client then prompts for and sends the message to be sent.

	Server receives the above information and checks to make sure the target user 
	exists/online.

	If the target user exists/online, the server forwards the message to the user. 

	The server should do this by sending the message to the corresponding socket 
	descriptor of the target user.

	Server sends confirmation that the message was sent or that the user did not exist.

	Client receives the confirmation from the server and prints it out.

	Client and server return to "prompt user for operation" and "wait for operation 
	from client" state respectively.

------------------------------------------------------------------------------------------------

E:	Client sends operation (E) to close its connection with the server and end the program.

	Server receives the operation and closes the socket descriptor for the client.

	Server updates its tracking record on the socket descriptors of active clients and 
	usernames of online users.

	Client should close socket and return.

------------------------------------------------------------------------------------------------

Build with 'make' in both client/ and server/.
to run the client:
	./chatclient Server_Name Port Username
to runt the server:
	./chatserver Port
