#include "streamer.h"
#include <signal.h>
#include <sys/poll.h>


bool Streamer::keepServing;
cv::VideoCapture Streamer::frameSource;
std::vector<std::thread> Streamer::workerList;



void Streamer::EndConnection() {
	Streamer::keepServing = false;
	for (int i=0; i<workerList.size(); ++i) {
		workerList[i].join();
	}
	
	shutdown(serverSocket, SHUT_RDWR);
	redirectorThread.join();
	close(serverSocket);
}



void Streamer::CreateConnection(string ip, string port) {
	struct sockaddr_in socketAddress;
	int set = 1;


	/* Creating a socket of desired type */
	serverSocket = socket(AF_INET, SOCK_STREAM, DEFAULT_OPTIONS);
	if (serverSocket == ERROR_CODE) {
		std::cerr << "\nError\nServer socket creation failed\n";
		exit(EXIT_FAILURE);
	}

	/* Changing socket options to set port as reuasble */
	if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &set, sizeof(int)) == ERROR_CODE) {
		std::cerr << "\nError\nReusable port options couldn't set\n";
		exit(EXIT_FAILURE);
	}

	/* Filling socket address structure */
	socketAddress.sin_family = AF_INET;
	socketAddress.sin_addr.s_addr = inet_addr(ip.c_str());
	socketAddress.sin_port = htons(stoi(port));

	/* Binding socket descriptor to socket address */
	if (bind(serverSocket, (struct sockaddr *)&socketAddress, sizeof(struct sockaddr)) == ERROR_CODE) {
		std::cerr << "\nError\nServer socket binding failed\n";
		exit(EXIT_FAILURE);
	}

	std::cout << "Server is ready to stream on " << ip << ":" << port << std::endl;
}



void Streamer::ListenConnectionPoint(int clientLimit) {
	/* Listening bound socket for maximum of given amount of clients */
	if (listen(serverSocket, clientLimit) == ERROR_CODE) {
		std::cerr << "\nError\nServer failed to listen bound socket\n";
		exit(EXIT_FAILURE);
	}

	Streamer::keepServing = true;
	redirectorThread = std::thread(AcceptClients, serverSocket);
	std::cout << "Client limit set to " << clientLimit << std::endl;
}



void Streamer::SetCaptureSource(cv::VideoCapture newSource) {
	frameSource = newSource;
}



void Streamer::AcceptClients(int socket) {
	int clientSocket;
	
	
	while(keepServing) {
		clientSocket = accept(socket, nullptr, nullptr);
		if (clientSocket == -1) {
			if (errno != EINVAL)
				std::cerr << "\nError\nConnection attempt with client failed\n" << errno << std::endl;
		}

		else {
			std::cout << "Creating new thread for client #" << clientSocket << std::endl;
			std::thread newThread(ServeClient, clientSocket);
			workerList.push_back(std::move(newThread));
		}
	}	
}



void Streamer::ServeClient(int client) {
	cv::Mat frame;
	int frameSize;
	
	
	while(Streamer::keepServing) {
		frameSource >> frame;
		frameSize = frame.total()*frame.elemSize();
		
		
		if (send(client, &frameSize, sizeof(frameSize), DEFAULT_OPTIONS) <= ZERO) {
			break;
		}
		
		if (send(client, &frame.cols, sizeof(frame.cols), DEFAULT_OPTIONS) <= ZERO) {
			break;
		}
		
		if (send(client, &frame.rows, sizeof(frame.rows), DEFAULT_OPTIONS) <= ZERO) {
			break;
		}
		
		if (send(client, frame.data, frameSize, DEFAULT_OPTIONS) <= ZERO) {
			break;
		}
	}
	
	
	std::cerr << "Connection closed with client #" << client << std::endl;
	close(client);
}
