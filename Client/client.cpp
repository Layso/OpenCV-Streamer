#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>

/* Using statements */
using std::cout;
using std::endl;
using std::cerr;
using cv::Mat;
using cv::VideoCapture;
using cv::waitKey;
using cv::Size;



/* Already constant values, defining just to give a name to them */
#define ZERO 0
#define ERROR_CODE -1
#define DEFAULT_OPTIONS 0
#define ESC_KEYCODE 27
#define MILISECONDS 1000
#define VIDEO_SOURCE 0



int main(int argc, char **argv){
	if (argc != 3) {
		std::cerr << "Usage: " << argv[0] << " ipv4Address portNo" << std::endl;
		exit(EXIT_FAILURE);
	}


	int bytesRead;
	int serverSocket;
	struct sockaddr_in serverAddress;
	struct hostent *server;

	Mat image = Mat::zeros(640,480, CV_8UC3);
	int frameSize = image.total() * image.elemSize();
	uchar frameData[frameSize];

	/* Creating client socket */
	serverSocket = socket(AF_INET, SOCK_STREAM, DEFAULT_OPTIONS);
	if (serverSocket == ERROR_CODE) {
		std::cerr << "\nError\nSocket creation for server is failed" << endl;
		exit(EXIT_FAILURE);
	}

	/* Filling socket address structure */
	server = gethostbyname(argv[1]);
	bcopy((char *)server->h_addr, (char*)&serverAddress.sin_addr.s_addr, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(atoi(argv[2]));

	/* Connecting to server */
	if (connect(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == ERROR_CODE) {
		std::cerr << "\nError\nConnection to server socket failed" << endl;
		exit(EXIT_FAILURE);
	}

	else {
		bool cont = true;
		cout << "Client connected to server\n";
		while (cont) {
			for (int i=0; i<frameSize; i+=bytesRead) {
				bytesRead = recv(serverSocket, frameData+i, frameSize-i, 0);
				if (bytesRead == 0) {
					std::cout << "Connection terminated by server\n";
					cont = false;
					break;
				}
			}

			Mat frame(Size(640,480), CV_8UC3, frameData);
			imshow("Client Stream", frame);
			if (waitKey(1) == 27) {
				cont = false;
			}
		}
	}

	close(serverSocket);




	/* C CLIENT
	int status;
	int fileDescriptor;
	int socketDescriptor;
	char buffer[BUFFER_SIZE];
	char fileName[] = "./Output/music.mp3";
	struct sockaddr_in socketAddress;


	/* Creating socket
	socketDescriptor = socket(AF_INET, SOCK_STREAM, DEFAULT_OPTIONS);
	if (socketDescriptor == ERROR_CODE) {
		fprintf(stderr, "\nAn error occured during creating socket\n%s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}


	/* Specifiying address informations for socket
	socketAddress.sin_family = AF_INET;
	socketAddress.sin_port = htons(9002);
	socketAddress.sin_addr.s_addr = INADDR_ANY;


	/* Connecting to socket
	status = connect(socketDescriptor, (struct sockaddr*) &socketAddress, sizeof(socketAddress));
	if (status == ERROR_CODE) {
		fprintf(stderr, "\nAn error occured during connecting to socket\n%s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	else {
		printf("Connection established with server. Recieveing message:\n----------\n");
	}


	/* Opening file to write data will be recieved from server
	fileDescriptor = open(fileName,  O_CREAT | O_WRONLY | O_APPEND, 0777);
	if (status == ERROR_CODE) {
		fprintf(stderr, "\nAn error occured during recieving message from server\n%s\n", strerror(errno));
		exit(EXIT_FAILURE);
		close(socketDescriptor);
	}


	/* Recieving server message
	do {
		printf("Akayın\n");
		memset(buffer, ZERO, BUFFER_SIZE);
		status = recv(socketDescriptor, buffer, 50, DEFAULT_OPTIONS);
		if (status == ERROR_CODE) {
			fprintf(stderr, "\nAn error occured during recieving message from server\n%s\n", strerror(errno));
			close(socketDescriptor);
			close(fileDescriptor);
			exit(EXIT_FAILURE);
		}
		printf("babanesi\n");
		status = write(fileDescriptor, &buffer, BUFFER_SIZE-1);
		if (status == ERROR_CODE) {
			fprintf(stderr, "\nAn errur occured during writing data to file\n%s\n", strerror(errno));
			close(socketDescriptor);
			close(fileDescriptor);
			exit(EXIT_FAILURE);
		}

		fprintf(stderr, "%d bytes recieved\n", status);
		printf("%d\n", status > ZERO);
		printf("%d\n", ZERO);
		printf("%d\n\n", status);
	} while (status > ZERO);


	/* Closing socket after message recieved
	printf("\n----------\nOperation done. Closing socket\n");
	close(socketDescriptor);
	close(fileDescriptor);*/
	return 0;
}
