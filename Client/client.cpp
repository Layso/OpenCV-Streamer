#include "client.h"



/* Static member definitions */
bool Client::keepGoing;
cv::Rect Client::selection;
cv::Mat Client::currentFrame;
#ifdef __linux__
int Client::serverSocket;
#elif _WIN32
SOCKET Client::serverSocket;
#endif

/* Returns the run status flag */
bool Client::Continues() {
	return keepGoing;
}



/* Returns true if client is a super user */
bool Client::SuperUser() {
	return userType == SUPER_USER;
}



/* Indicates the service is over and waits for reciever thread to join */
void Client::EndConnection() {
	keepGoing = false;
	recieverThread.join();
}



/* Getter for the last recieved frame from the server */
cv::Mat Client::GetFrame() {
	return currentFrame;
}



/*  */
void Client::NewCommand(struct ClientMessage msg) {
	std::thread messenger(SendCommand, msg);
	messenger.detach();
}



/* Thread function to send the new command to the server through socket */
void Client::SendCommand(struct ClientMessage msg) {
	int bytes;
	
	
	#ifdef __linux__
	bytes = send(serverSocket, &msg, sizeof(msg), DEFAULT_OPTIONS);
	if (bytes <= ZERO) {
		std::cerr << "New command couldn't sent to server\n";	
	}
	
	else {
		std::cout << "New command sent to the server\n";
	}
	#elif _WIN32
	bytes = send(serverSocket, (char*)&msg, sizeof(msg), DEFAULT_OPTIONS);
	if (bytes == ZERO || bytes == SOCKET_ERROR) {
		std::cerr << "New command couldn't sent to server\n"
	}
	
	else {
		std::cout << "New command sent to the server\n";
	}
	#endif
}



/*  */
void Client::CreateConnection(std::string ip, std::string port, int mode) {
#ifdef __linux__
	struct hostent *server;
	struct sockaddr_in serverAddress;


	// Creating server socket for connection
	serverSocket = socket(AF_INET, SOCK_STREAM, DEFAULT_OPTIONS);
	if (serverSocket == ERROR_CODE) {
		std::cerr << "\nSystem Error\nServer socket creation failed\n";
		exit(EXIT_FAILURE);
	}

	// Filling server address structure
	server = gethostbyname(ip.c_str());
	bcopy((char *)server->h_addr, (char*)&serverAddress.sin_addr.s_addr, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(atoi(port.c_str()));

	// Connecting to server
	if (connect(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == ERROR_CODE) {
		std::cerr << "\nSystem Error\nConnection to server socket failed" << std::endl;
		exit(EXIT_FAILURE);
	}
#elif _WIN32
	int error;
	u_long iMode = 1;
	WSADATA wsadata;
	SOCKADDR_IN address;


	// Starting WSA
	error = WSAStartup(WSADATA_VERSION, &wsadata);
	if (error) {
		std::cerr << "\nSystem Error\nWSA startup failed" << std::endl;
		exit(EXIT_FAILURE);
	}

	// Checking WSA version
	if (wsadata.wVersion != WSADATA_VERSION) {
		WSACleanup();
		std::cerr << "\nSystem Error\nWSA version check failed" << std::endl;
		exit(EXIT_FAILURE);
	}

	// Filling server address structure
	address.sin_family = AF_INET; // address family Internet
	address.sin_port = htons(atoi(port.c_str())); //Port to connect on
	address.sin_addr.S_un.S_addr = inet_addr(ip.c_str()); //Target IP

	// Creating server socket for connection
	serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serverSocket == INVALID_SOCKET) {
		std::cerr << "\nSystem Error\nConnection to server socket failed" << std::endl;
		exit(EXIT_FAILURE);
	}


	// Connecting to server
	if (connect(serverSocket, (SOCKADDR *)&address, sizeof(address)) == SOCKET_ERROR) {
		std::cerr << "\nSystem Error\nConnection to server socket failed" << std::endl;
		exit(EXIT_FAILURE);
	}
#endif
	
	
	// Creating reciever thread
	keepGoing = true;
	userType = mode;
	recieverThread = std::thread(RecieveFrames);
	std::cout << "Connection established with server\n";
}






/* Thread function to continuously recieve frames and set to local frame member */
void Client::RecieveFrames() {
	int i;
	int bytesRead;
	char *frameData;
	uint32_t size = ZERO;
	char buffer[sizeof(size)];
	std::vector<char> feriha;
	
	
	// Reading frame continuously from the socket
	while (keepGoing) {
		// Reading frame size first to read and form the frame
		#ifdef __linux__
		bytesRead = recv(serverSocket, &size, sizeof(size), 0);
		if (bytesRead <= ZERO) {
			std::cout << "Connection terminated by server\n";
			keepGoing = false;
			break;
		}
		#elif _WIN32
		std::memset(buffer, ZERO, sizeof(size));
		bytesRead = recv(serverSocket, buffer, sizeof(size), DEFAULT_OPTIONS);
		if (bytesRead == ZERO || bytesRead == SOCKET_ERROR) {
			std::cout << "Connection terminated by server\n";
			keepGoing = false;
			break;
		}
		else {
			size = *(uint32_t*)buffer;
		}
		#endif
		
		
		// Reading frame data and forming a Mat object that holds the compressed frame
		frameData = new char[size];
		for (i = 0; i<size; i += bytesRead) {
			#ifdef __linux__
			bytesRead = recv(serverSocket, frameData + i, size - i, 0);
			if (bytesRead <= ZERO) {
				std::cout << "Connection terminated by server\n";
				keepGoing = false;
				break;
			}
			#elif _WIN32
			bytesRead = recv(serverSocket, frameData + i, size - i, 0);
			if (bytesRead == ZERO || bytesRead == SOCKET_ERROR) {
				std::cout << "Connection terminated by server\n";
				keepGoing = false;
				break;
			}
			#endif
		}
		
		// Converting char array to vector for cv::Mat transformation
		for (i = 0; i<size; ++i) {
			feriha.push_back(frameData[i]);
		}
		
		cv::Mat bufferFrame(feriha);
		if (!bufferFrame.empty() && bufferFrame.isContinuous()) {
			cv::imdecode(bufferFrame, CV_LOAD_IMAGE_COLOR).copyTo(currentFrame);
		}
		feriha.clear();
		delete frameData;
	}
}