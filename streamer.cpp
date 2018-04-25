#include "streamer.h"
#include <signal.h>


bool Streamer::keepServing;
cv::Mat Streamer::frame;
cv::VideoCapture Streamer::cap;


Streamer::Streamer() {
  sentCount = 0;
  serverSocket = 0;
  maximumClients = 0;
}



void Streamer::EndConnection() {
  Streamer::keepServing = false;
  /* TODO: Join redirectorThread */
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
  redirectorThread = thread(AcceptClients, serverSocket);
  std::cout << "Server socket connection established\n";
  std::cout << "Client limit set to " << clientLimit << std::endl;
}



void Streamer::SendFrame(cv::Mat newFrame) {
  newFrame.copyTo(frame);
}



void Streamer::AcceptClients(int socket) {
  std::vector<thread> workerList;
  int currentClient = 0;
  int clientSocket;

  while(true) {
    clientSocket = accept(socket, nullptr, nullptr);
    if (clientSocket == -1) {
      std::cerr << "\nError\nConnection attempt with client failed\n";
    }

    else {
      std::cout << "Creating new thread for client #" << clientSocket << std::endl;
      thread newThread(ServeClient, clientSocket);
      workerList.push_back(std::move(newThread));
    }
  }


  for (int i=0; i< workerList.size(); ++i) {
    workerList[i].join();
  }
}


void Streamer::ServeClient(int client) {
  while(Streamer::keepServing) {
    int frameSize = frame.total()*frame.elemSize();
    send(client, frame.data, frameSize, 0);
  }

  std::cout << "Closing client socket #" << client << std::endl;
  close(client);
}
