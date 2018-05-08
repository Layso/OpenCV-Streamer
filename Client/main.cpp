#include "client.h"



#define EXE_NAME_INDEX 0
#define IP_ADDRESS_INDEX 1
#define PORT_INDEX 2
#define USER_MODE_INDEX 3
#define REQUIRED_ARGC 4
#define ESC_KEYCODE 27



Client *client = nullptr;



static void MouseEventWrapper( int event, int x, int y, int, void*) {
    if (client != nullptr && client->SuperUser())
        client->MouseEvent(event, x, y);
}



int main(int argc, char **argv) {
	if (argc != REQUIRED_ARGC) {
		std::cerr << "Usage: " << argv[EXE_NAME_INDEX] << " [ipAddress] [port] [userType]\n";
		return EXIT_FAILURE;
	}

    
    /* Preparation and initializiations */
    bool cont = true;
    client = new Client();
	client->CreateConnection(argv[IP_ADDRESS_INDEX], argv[PORT_INDEX], atoi(argv[USER_MODE_INDEX]));
    cv::namedWindow("Client Stream", CV_WINDOW_AUTOSIZE);
	cv::setMouseCallback("Client Stream", MouseEventWrapper, DEFAULT_OPTIONS);
    
    /* Infinite (until ESC pressed) loop to print recieved frames */
	while (cont) {
		if (client->GetFrame().data != NULL) {
			imshow("Client Stream", client->GetFrame());
		}

		if (cv::waitKey(1) == ESC_KEYCODE) {
			cont = false;
		}
	}
    
    /* Cleaning up */
    client->EndConnection();
    delete client;
	return EXIT_SUCCESS;
}