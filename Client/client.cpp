#include "client.h"



/* Static member definitions */
cv::Mat Client::currentFrame;
bool Client::keepGoing;
cv::Rect Client::selection;



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


	if(selectObject) {
		selection.x = MIN(x, firstPoint.x);
		selection.y = MIN(y, firstPoint.y);
		selection.width = std::abs(x - firstPoint.x);
		selection.height = std::abs(y - firstPoint.y);
		selection &= cv::Rect(0, 0, currentFrame.cols, currentFrame.rows);
		cv::rectangle(currentFrame, cvPoint(firstPoint.x, firstPoint.y), cvPoint(x, y), cvScalar(255, 255, 255));
	}

	switch(event) {
		case cv::EVENT_LBUTTONDOWN:
			firstPoint = cv::Point(x,y);
			secondPoint = cv::Point(x,y);
			selection = cv::Rect(x,y,0,0);
			selectObject = true;
			break;

		case cv::EVENT_LBUTTONUP:
			secondPoint = cv::Point(x,y);
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
		bytes = send(socket, &selection, sizeof(selection), DEFAULT_OPTIONS);
		std::cout << "New selection sent to the server\n";
	}
}



/* Creates a connection with given ip and port settings between server */
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
	userType = mode;
	keepGoing = true;
	recieverThread = std::thread(RecieveFrames, serverSocket);
	std::cout << "Connection established with server\n";
}



/* Thread function to continuously recieve frames and set to local frame member */
void Client::RecieveFrames(int socket) {
	int i;
	int size;
	int bytesRead;
	int width, height;
	uchar *frameData = nullptr;


	/* Reading frame continuously from the socket */
	while (keepGoing) {
		/* Reading frame size, width and length first to read and form the frame */

		bytesRead = recv(socket, &size, sizeof(size), 0);
		if (bytesRead <= 0) {
			std::cout << "Connection terminated by server\n";
			keepGoing = false;
			break;
		}

		bytesRead = recv(socket, &width, sizeof(width), 0);
		if (bytesRead <= 0) {
			std::cout << "Connection terminated by server\n";
			keepGoing = false;
			break;
		}

		bytesRead = recv(socket, &height, sizeof(height), 0);
		if (bytesRead <= 0) {
			std::cout << "Connection terminated by server\n";
			keepGoing = false;
			break;
		}

		/* Finally reading data and forming a Mat object that holds the frame */
		bytesRead = ZERO;
		uchar frameData[size];

		for (int i=0; i<size; i+=bytesRead) {
			bytesRead = recv(socket, frameData+i, size-i, 0);
			if (bytesRead <= 0) {
				std::cout << "Connection terminated by server\n";
				keepGoing = false;
				break;
			}
		}


		cv::Mat imgbuf(cv::Size(width, height), CV_8UC3, frameData);
		imgbuf = cv::imdecode(imgbuf, CV_LOAD_IMAGE_COLOR).clone();
		imgbuf.copyTo(currentFrame);

		/*

		*/
		/* Encode compressed image data and assign to current frame */
		/* Setting new frame to local frame */
		/*
		currentFrame = cv::Mat(cv::Size(width,height), CV_8UC3, data);

		if (frameData == nullptr) {
			std::cout << "REEEEEEEEEEEEEEE3\n";
		}
		frameData = nullptr;

		std::cout << "Rcv 1\n";
		std::cout << "Rcv 2\n";
		std::cout << "Rcv 3\n";
		std::cout << "Rcv 4\n";
		std::cout << "Rcv 5\n";
		std::cout << "Rcv 6\n";
		std::cout << "Rcv 6.5\n";
		std::cout << "Rcv 7\n";

		*/
	}


	std::cout << "Connection closed with server\n";
	close(socket);
}
