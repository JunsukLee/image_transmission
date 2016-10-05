#ifndef TRANSFER_IMAGE_H
#define TRANSFER_IMAGE_H

#include "opencv2/opencv.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

#define SERVER_PORT 2223
#define SERVER_IP   "203.230.103.47"

using namespace std;
//using namespace cv;

class Transfer_image
{
public:
	Transfer_image();
	~Transfer_image();
	void init();
	void start_video();		
	bool set_on_transmission();
	bool set_off_transmission();
	void socket_connected();
	void neg_status_recovery();

public:


private:
	void get_currentFrame(cv::Mat frame);	
	void socket_transmission(std::string image_String);
	void socket_receive();
	void socket_close();
	void video_close();
	char itoa(int num);


private:
	cv::VideoCapture cap;
	cv::Mat tagetFrame;
	int id;

	bool status_transmission;
	cv::Size size;
	int imageSize_Total;

	//Socket Setting
	int	client_socket;
	struct	sockaddr_in server_addr;
	bool    ready;
};

#endif //TRANSFER_IMAGE_H
