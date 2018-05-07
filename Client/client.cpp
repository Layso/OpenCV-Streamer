#include "client.h"



/* Static member definitions */
cv::Mat Client::currentFrame;
bool Client::keepGoing;



bool Client::Continues() {
	return keepGoing;
}



void Client::EndConnection() {
	keepGoing = false;
	recieverThread.join();
}



cv::Mat Client::GetFrame() {
	return currentFrame;
}



void Client::CreateConnection(std::string ip, std::string port, int mode) {
	struct hostent *server;
	struct sockaddr_in serverAddress;

	
	/* Creating server socket for connection */
	serverSocket = socket(AF_INET, SOCK_STREAM, DEFAULT_OPTIONS);
	if (serverSocket == ERROR_CODE) {
		std::cerr << "\nSystem Error\nServer socket creation failed\n";
		exit(EXIT_FAILURE);
	}
	
	/* Preparing socket address */
	server = gethostbyname(ip.c_str());
	bcopy((char *)server->h_addr, (char*)&serverAddress.sin_addr.s_addr, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(atoi(port.c_str()));
	
	/* Connecting to server */
	if (connect(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == ERROR_CODE) {
		std::cerr << "\nError\nConnection to server socket failed" << std::endl;
		exit(EXIT_FAILURE);
	}
	
	
	/* Creating thread to recieve frames */
	keepGoing = true;
	recieverThread = std::thread(RecieveFrames, serverSocket);
	std::cout << "Connection established with server\n";
}



void Client::RecieveFrames(int socket) {
	int i;
	int bytesRead;
	int frameSize;
	int width, height;
	
	
	/* Reading frame continuously from the socket */
	while (keepGoing) {
		/* Reading frame size from socket */
		bytesRead = recv(socket, &frameSize, sizeof(frameSize), 0);
		if (bytesRead <= 0) {
			std::cout << "Connection terminated by server\n";
			keepGoing = false;
			break;
		}
		
		/* Reading width, height and image data */
		bytesRead = 0;
		uchar frameData[frameSize];
		recv(socket, &width, sizeof(width), 0);
		recv(socket, &height, sizeof(height), 0);
		for (int i=0; i<frameSize; i+=bytesRead) {
			bytesRead = recv(socket, frameData+i, frameSize-i, 0);
		}
		
		/* Setting new frame to local frame */
		currentFrame = cv::Mat(cv::Size(width,height), CV_8UC3, frameData);
	}
	
	
	std::cout << "Connection closed with server\n";
	close(socket);
}