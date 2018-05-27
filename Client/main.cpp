#include "client.h"



#define EXE_NAME_INDEX 0
#define IP_ADDRESS_INDEX 1
#define PORT_INDEX 2
#define USER_MODE_INDEX 3
#define REQUIRED_ARGC 4
#define ESC_KEYCODE 27



Client *client = nullptr;
bool selectObject;
cv::Mat frame;
cv::Point firstPoint;
cv::Point secondPoint;
cv::Rect selection;
int firstX, firstY;


static void MouseEventWrapper( int event, int x, int y, int, void*) {
	switch( event ) {
		case CV_EVENT_LBUTTONDOWN:
			firstPoint = cv::Point(x, y);
			selectObject = true;
			break;
			
		case CV_EVENT_LBUTTONUP:
			selectObject = false;
			struct ClientMessage msg;
			secondPoint = cv::Point(x,y);
			
			int width = std::abs(firstPoint.x - x);
			int height = std::abs(firstPoint.y - y);
			
			if (width > ZERO && height > ZERO) {
				firstPoint.x = MIN(firstPoint.x, x);
				firstPoint.y = MIN(firstPoint.y, y);
				msg.message = MESSAGE_NEW_SELECTION;
				msg.selection = cv::Rect(firstPoint.x, firstPoint.y, width, height);
				msg.selection &= cv::Rect(ZERO, ZERO, frame.cols, frame.rows);
				client->NewCommand(msg);
			}
			break;
	}
}




int main(int argc, char **argv) {
	if (argc != REQUIRED_ARGC) {
		std::cerr << "Usage: " << argv[EXE_NAME_INDEX] << " [ipAddress] [port] [userType]\n";
		return EXIT_FAILURE;
	}


	/* Preparation and initializiations */
	bool cont = true;
	struct ClientMessage mesaj;
	client = new Client();
	client->CreateConnection(argv[IP_ADDRESS_INDEX], argv[PORT_INDEX], atoi(argv[USER_MODE_INDEX]));
	cv::namedWindow("Client Stream", CV_WINDOW_AUTOSIZE);
	cv::setMouseCallback("Client Stream", MouseEventWrapper, DEFAULT_OPTIONS);


	/* Infinite (until ESC pressed) loop to print recieved frames */
	while (cont) {
		client->GetFrame().copyTo(frame);
		if (frame.data != NULL && !selectObject) {
			imshow("Client Stream", frame);
		}
		
		
		switch(cv::waitKey(1)) {
			case ESC_KEYCODE:
				cont = false;
				break;
			
			case MESSAGE_MANUEL_MODE:
				mesaj.message = MESSAGE_MANUEL_MODE;
				client->NewCommand(mesaj);
				break;
					
			case MESSAGE_BLUETOOTH_MODE:
				mesaj.message = MESSAGE_BLUETOOTH_MODE;
				client->NewCommand(mesaj);
				break;
			
			case MESSAGE_STOP:
				mesaj.message = MESSAGE_STOP;
				client->NewCommand(mesaj);
				break;
			
			case MESSAGE_QUIT:
				mesaj.message = MESSAGE_QUIT;
				client->NewCommand(mesaj);
				break;
		}
	}

	/* Cleaning up */
	client->EndConnection();
	delete client;
	return EXIT_SUCCESS;
}
