#include "client.h"



/* Static member definitions */
bool Client::keepGoing;
cv::Rect Client::selection;
cv::Mat Client::currentFrame;



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



/* Mouse event function to select are from the stream and send to server to set as new track object */
void Client::MouseEvent(int event, int x, int y) {
	/* If selection flag is raised then the mouse currently pressed and dragged. Update and et new
	positions of the selection are for client to see where is selected. Draw a rectangle for visual guidance */
	/* If button pressed, set start points and raise selection flag */
	/* If button released, set end points and send the selection to the server */


	if (selectObject) {
		selection.x = MIN(x, firstPoint.x);
		selection.y = MIN(y, firstPoint.y);
		selection.width = std::abs(x - firstPoint.x);
		selection.height = std::abs(y - firstPoint.y);
		selection &= cv::Rect(0, 0, currentFrame.cols, currentFrame.rows);
		cv::rectangle(currentFrame, cvPoint(firstPoint.x, firstPoint.y), cvPoint(x, y), cvScalar(255, 255, 255));
	}

	switch (event) {
	case cv::EVENT_LBUTTONDOWN:
		firstPoint = cv::Point(x, y);
		secondPoint = cv::Point(x, y);
		selection = cv::Rect(x, y, 0, 0);
		selectObject = true;
		break;

	case cv::EVENT_LBUTTONUP:
		secondPoint = cv::Point(x, y);
		selectObject = false;
		std::thread Thread(SendSelection, serverSocket);
		Thread.detach();
		break;
	}
}



/* Thread function to send the selected are to the server through socket */
void Client::SendSelection(int socket) {
	int bytes;

	if (selection.width == 0 || selection.height == 0) {
		std::cerr << "Invalid size of selection\n";
	}

	else {
#ifdef __linux__
		bytes = send(socket, &selection, sizeof(selection), DEFAULT_OPTIONS);
#elif _WIN32
		bytes = send(socket, (char*)&selection, sizeof(selection), DEFAULT_OPTIONS);
#endif
		std::cout << "New selection sent to the server\n";
	}
}



/* Creating server socket for connection */
/* Preparing socket address */
/* Connecting to server */
/* Creating thread to recieve frames */
/* Creates a connection with given ip and port settings between server */
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

	// Creating reciever thread
	keepGoing = true;
	recieverThread = std::thread(RecieveFramesPOSIX, serverSocket);
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

	// Creating reciever thread
	keepGoing = true;
	recieverThread = std::thread(RecieveFramesWIN, serverSocket);
#endif

	userType = mode;
	std::cout << "Connection established with server\n";
}






/* Thread function to continuously recieve frames and set to local frame member */
#ifdef __linux__
void Client::RecieveFramesPOSIX(int socket) {
	int i;
	uint32_t size;
	int bytesRead;
	int width, height;
	char *frameData;
	std::vector<char> feriha;


	// Reading frame continuously from the socket
	while (keepGoing) {
		// Reading frame size first to read and form the frame
		bytesRead = recv(socket, &size, sizeof(size), 0);
		if (bytesRead <= ZERO) {
			std::cout << "Connection terminated by server " << bytesRead << " read\n";
			keepGoing = false;
			break;
		}

		// Reading frame data and forming a Mat object that holds the compressed frame
		frameData = new char[size];
		for (i = 0; i<size; i += bytesRead) {
			bytesRead = recv(socket, frameData + i, size - i, 0);
			if (bytesRead <= 0) {
				std::cout << "Connection terminated by server\n";
				keepGoing = false;
				break;
			}
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


	std::cout << "Connection closed with server\n";
	close(socket);
}
#elif _WIN32
void Client::RecieveFramesWIN(SOCKET socket) {
	int i;
	int bytesRead;
	uint32_t size = ZERO;
	char *frameData;
	char buffer[sizeof(size)];
	std::vector<char> feriha;


	// Reading frame continuously from the socket
	while (keepGoing) {
		// Reading frame size first to read and form the frame
		std::memset(buffer, ZERO, sizeof(size));
		bytesRead = recv(socket, buffer, sizeof(size), DEFAULT_OPTIONS);
		if (bytesRead == ZERO || bytesRead == SOCKET_ERROR) {
			std::cout << "Connection terminated by server\n";
			keepGoing = false;
			break;
		}
		else {
			size = *(uint32_t*)buffer;
		}

		// Reading frame data and forming a Mat object that holds the compressed frame
		frameData = new char[size];
		for (i = 0; i<size; i += bytesRead) {
			bytesRead = recv(socket, frameData + i, size - i, 0);
			if (bytesRead == ZERO || bytesRead == SOCKET_ERROR) {
				std::cout << "Connection terminated by server\n";
				keepGoing = false;
				break;
			}
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


	std::cout << "Connection closed with server\n";
	closesocket(socket);
}
#endif
