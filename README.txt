-Leoni Lu
-jl5592

Command line instructions: (please be sure every file is in the same directory)
make clean //clean all unnecessary files in the directory
make //would compile all programs and make UdpChat executable file
./UdpChat -s <port number> //the ip for the server would be shown
// go to another command window
./UdpChat -c <client name> <server ip> <server port> <client port>


Documents for the chatapp:
	UdpChat.c
	client.c
	server.c
	func.c
	func.h
	Makefile
	table

Main data structure:
	struct to hold userinformation
		struct usrinfo {
			char name[20];
			char ip[16];
			int port;
			int status;
		};
	struct to hold historical message
		struct mess {
			char from[20];
			char to[20];
			char time[10];
			char msg[200];
		};



