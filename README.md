# Trivial-File-Transfer-Protocol
The Trivial File Transfer Protocol (TFTP) is a protocol layered on the User Datagram transport Protocol (UDP) used over the Internet Protocol.  It also displays information as the transfer takes place, showing you the packet sizes being sent, the ACK receiving, etc. The number of retries of a data packet is set to be 3 in the code. It can be changed by the user. 

A simple implementation of the trivial file transfer protocol. Client sends a file request to the server. The server looks up the file and replies with packets of file of size 512 bytes.

To execute the program specify a port number between 1024 and 65535. The same port numer has to be specified for both client and server. After executing makefile,

	execute ./client PORT_NUMBER
	execute ./server PORT_NUMBER

Client will prompt the user for the file name, specify the path to the file.
