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

using std::string;
using std::thread;



class Streamer {
public:
  /* Constructor */
  Streamer();

  /* Public functions to operate */
  void CreateConnection(string ip, string port);
  void ListenConnectionPoint(int clientLimit);
  void SendFrame(cv::Mat newFrame);
  void EndConnection();

private:
  /* Member variables */
  int serverSocket;
  int socketType;
  int sentCount;
  int maximumClients;
  thread redirectorThread;

  /* Static members to access from threads */
  static bool keepServing;
  static cv::Mat frame;
  static cv::VideoCapture cap;

  /* Class specific constant values */
  const int ZERO = 0;
  const int ERROR_CODE = -1;
  const int DEFAULT_OPTIONS = 0;

  /* Helper functions */
  static void AcceptClients(int socket);
  static void ServeClient(int client);
};





#endif