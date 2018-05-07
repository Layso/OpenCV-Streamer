#include "client.h"



#define EXE_NAME_INDEX 0
#define IP_ADDRESS_INDEX 1
#define PORT_INDEX 2
#define USER_MODE_INDEX 3
#define REQUIRED_ARGC 4
#define ESC_KEYCODE 27



int main(int argc, char **argv) {
	if (argc != REQUIRED_ARGC) {
		std::cerr << "Usage: " << argv[EXE_NAME_INDEX] << " [ipAddress] [port] [userType]\n";
		return EXIT_FAILURE;
	}
	
	
	Client client;
	client.CreateConnection(argv[IP_ADDRESS_INDEX], argv[PORT_INDEX], atoi(argv[USER_MODE_INDEX]));
	while (client.Continues()) {
        if (client.GetFrame().data != NULL) {
            imshow("Client Stream", client.GetFrame());
        }
        
        if (cv::waitKey(1) == ESC_KEYCODE) {
            client.EndConnection();
        }
	}
    
    
	return EXIT_SUCCESS;
}

/*
l backprojMode = false;
bool selectObject = false;
int trackObject = 0;
bool showHist = true;
cv::Point origin;
cv::Rect selection;
int vmin = 10, vmax = 256, smin = 30;



cv::Point clickStart;
cv::Point clickEnd;



void Printer() {
    std::cout << clickStart.x << "/" << clickStart.y << "   ---   " << clickEnd.x << "/" << clickEnd.y << std::endl;
    selection.x = MIN(clickStart.x, clickEnd.x);
    selection.y = MIN(clickStart.y, clickEnd.y);
    selection.width = std::abs(clickStart.x - clickEnd.x);
    selection.height = std::abs(clickStart.y - clickEnd.y);
}

static void onMouse( int event, int x, int y, int, void* ) {
    if (selectObject) {
        clickEnd = cv::Point(x, y);    
    }
    
    
    switch (event) {
        case cv::EVENT_LBUTTONDOWN:
            selectObject = true;
            clickEnd = cv::Point(x, y);
            clickStart = cv::Point(x, y);
            Printer();
            break;
        
        case cv::EVENT_LBUTTONUP:
            selectObject = false;
            clickEnd = cv::Point(x, y);
            Printer();
            break;
    }
}


int main(int argc, char **argv) {
	cv::Mat frame;
    cv::Mat newFrame;
	cv::VideoCapture frameSource;
	std::vector<int> params;
    std::vector<uchar> buffer;
    
    
	frameSource.open(0);
	cv::namedWindow("Video", CV_WINDOW_AUTOSIZE);
    cv::namedWindow("Zipped/Unzipped Video", CV_WINDOW_AUTOSIZE);
	cv::setMouseCallback("Video", onMouse, 0 );
    params.push_back(CV_IMWRITE_JPEG_QUALITY);
    params.push_back(100);
    
    
	
    while(1) {
		frameSource >> frame;
        cv::imshow("Video", frame);
        
        if (clickEnd.x != 0 && frame.data != NULL)
            cv::rectangle(frame, cvPoint(clickStart.x, clickStart.y), cvPoint(clickEnd.x, clickEnd.y), cvScalar(255, 255, 255));
		cv::waitKey(5);
        
        std::cout << "Current size: " << frame.elemSize() * frame.total() << std::endl;
        
        cv::imencode(".jpg", frame, buffer, params);
        std::cout << "Compressed s: " << buffer.size() << std::endl;
        
        
        cv::imdecode(buffer, CV_LOAD_IMAGE_COLOR, &newFrame);
		cv::imshow("Zipped/Unzipped Video", newFrame);
    }
	
	
	return 0;
}
*/