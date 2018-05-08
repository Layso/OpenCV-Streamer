#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/video/tracking.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <thread>

#include "opencv2/highgui.hpp"



#include <opencv2/core/utility.hpp>
#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"




/* Already constant values, defining just to give a name to them */
#define NORMAL_USER 0
#define SUPER_USER 1
#define ZERO 0
#define ERROR_CODE -1
#define DEFAULT_OPTIONS 0
#define ESC_KEYCODE 27
#define MILISECONDS 1000
#define VIDEO_SOURCE 0
	


class Client {
public:
	bool Continues();
	bool SuperUser();
	cv::Mat GetFrame();
	void EndConnection();
	void MouseEvent(int event, int x, int y);
	void CreateConnection(std::string ip, std::string port, int mode);
	
private:
	int userType;
	bool continues;
	int serverSocket;
	bool selectObject;
	cv::Point firstPoint;
	cv::Point secondPoint;
	std::thread recieverThread;
	
	/* Helper private functions */
	
	/* Static functions for threads */
	static void SendSelection(int socket);
	static void RecieveFrames(int socket);
	
	/* Static member variables for threads */
	static cv::Rect selection;
	static cv::Mat currentFrame;
	static bool keepGoing;
};

#endif