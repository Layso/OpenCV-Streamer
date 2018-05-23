#ifndef STREAMER_H
#define STREAMER_H

#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <thread>
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



using std::string;



#define ZERO 0
#define ERROR_CODE -1
#define DEFAULT_OPTIONS 0



class Streamer {
public:  
	/* Public functions to operate */
	void SetCaptureSource(cv::VideoCapture newSource);
	void CreateConnection(std::string port);
	void ListenConnectionPoint(int clientLimit);
	void SendFrame(cv::Mat newFrame);
	void EndConnection();
	bool SelectionChanged();
	cv::Rect GetNewSelection();

private:
	/* Member variables */
	int serverSocket;
	std::thread redirectorThread;

	/* Static members to access from threads */
	static bool keepServing;
	static bool selectionRecieved;
	static cv::Rect currentSelection;
	static cv::VideoCapture frameSource;
	static std::vector<std::thread> workerList;
	/* Helper functions */
	static void RecieveSelection(int socket);
	static void AcceptClients(int socket);
	static void ServeClient(int client);
};





#endif
