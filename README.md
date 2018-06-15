# About OpenCV-Streamer
This repository contains a server and a client program for CSE396 CSE Project course. The server contains 2 seperate cpp folders: camshiftdemo and streamer. The streamer is a class implemented for the data transfer to the clients. camshiftdemo is an example of the OpenCV library for object tracking. Client is a program that uses same protocol as streamer to recieve the video stream frame by frame from server and display on a window. Client can also send selection to the server to follow an object on the frame or stop tracking.

# Compilation
Makefiles (different for both) can be used to compile the programs. It only requires OpenCV library and POSIX supported OS for the server. Client can be built on both POSIX supported operating systems and Windows. If prerequisites are provided, 'make' command will be enough to compile and get the executables.

# Running
To run the programs 'make run' commands can be used. It is suggested to edit the 'run' rules of the makefiles for more consistent run time experience. Running programs without the required arguments will result the print of usage which informs the user about how to run the program.
