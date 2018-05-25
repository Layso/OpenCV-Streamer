#include <iostream>
#include <string>
#include <thread>
#include <stdlib.h>
#include <sys/types.h>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/video/tracking.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>


#ifdef __linux__
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#elif _WIN32
#pragma once
#include <winsock.h>
#pragma comment(lib,"WS2_32")
#endif


#define NORMAL_USER 0
#define SUPER_USER 1
#define ZERO 0
#define ERROR_CODE -1
#define DEFAULT_OPTIONS 0
#define ESC_KEYCODE 27
#define MILISECONDS 1000
#define VIDEO_SOURCE 0
#define WSADATA_VERSION 0x0202
#define BUFFER_SIZE sizeof(uint16_t)



class Client
{
public:
	bool Continues();
	bool SuperUser();

	cv::Mat GetFrame();
	void EndConnection();
	void MouseEvent(int event, int x, int y);
	void CreateConnection(std::string ip, std::string port, int mode);

private:
	// Member variables
	int userType;
	bool continues;
	bool selectObject;
	cv::Point firstPoint;
	cv::Point secondPoint;
	std::thread recieverThread;

	// OS specific declarations
#ifdef __linux__
	int serverSocket;
	static void RecieveFramesPOSIX(int socket);
#elif _WIN32
	SOCKET serverSocket;
	static void RecieveFramesWIN(SOCKET socket);
#endif

	// Static functions for threads
	static void SendSelection(int socket);

	// Static member variables for threads
	static bool keepGoing;
	static cv::Rect selection;
	static cv::Mat currentFrame;
};




