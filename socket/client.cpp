#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <iostream>
#include "opencv2/opencv.hpp"

// refer : dataisze_lenght (921600)

#define  FRAME_SIZE  921600
#define  BUFF_MAX    32768 // 65535byte
#define  BUFF_SIZE   1024
#define  PORT        2223
#define  SERVER_IP   "203.230.103.47"

//sudo g++ client.cpp Transr_image.cpp -o client `pkg-config --libs --cflags opencv` -std=c++11 -fpermissive

std::string frameTostring(cv::Mat frame);
char itoa(int num);
int  atoi_(char c);

using namespace std;
//using namespace cv;

int   main( int argc, char **argv)
{
   int   client_socket;
   int   server_addr_size;

   struct sockaddr_in   server_addr;

   char   buff_rcv[BUFF_SIZE];
   std::string str_frame;
   int    segment_count;

   char   buff_s[BUFF_MAX];

   //Video
   cv::VideoCapture cap;
   cv::Mat          currentFrame;

   cap.open(0);
   if(!cap.isOpened()){
	printf("Connecting Video Cam is Failed!!\n");
	return 0;
   }


   client_socket  = socket( PF_INET, SOCK_DGRAM, 0);   
   
   if( -1 == client_socket)
   {
      printf( "socket create fail\n");
      exit( 1);
   }

   memset( &server_addr, 0, sizeof( server_addr));
   server_addr.sin_family     = AF_INET;
   server_addr.sin_port       = htons(PORT);
   server_addr.sin_addr.s_addr= inet_addr(SERVER_IP);

   segment_count = 921600 / BUFF_MAX;
   (921600 == (segment_count * segment_count))?:segment_count++;

   cv::namedWindow("Video_Client", CV_WINDOW_AUTOSIZE);
   while(1){
	str_frame.erase(0, str_frame.length());
	cap >> currentFrame;
	std::cout << "rows : " << currentFrame.rows << std::endl;
	std::cout << "cols : " << currentFrame.cols << std::endl;
        imshow("Video_server", currentFrame);
        if(cv::waitKey(120) >= 0)
		break;
	str_frame = frameTostring(currentFrame);
	printf("str_frame length : %ld\n", str_frame.length());
	printf("segment_count    : %d\n", segment_count);

	for(int i=0, j=0; i < segment_count; i++){ //id(2) total(2) current_id(2) size(4) purry(-)
		buff_s[0] = 'D';
		buff_s[1] = 'A';

		buff_s[2] = itoa(segment_count/10);		
		buff_s[3] = itoa(segment_count%10);

		buff_s[4] = itoa(i/10);
		buff_s[5] = itoa(i%10);
	
		for(j=0; j < (BUFF_MAX-10); j++){
			if((BUFF_MAX-10)*i+j >= FRAME_SIZE)
				break;
			buff_s[j+10] = str_frame.at((BUFF_MAX-10)*i+j);
		}
		int temp=j;
		int mod, mok;
		mok = temp/16;	mod = temp - mok*16;	temp = mok;
		buff_s[9] = itoa(mod);
		mok = temp/16;	mod = temp - mok*16;	temp = mok;
		buff_s[8] = itoa(mod);
		mok = temp/16;	mod = temp - mok*16;	temp = mok;
		buff_s[7] = itoa(mod);
		mok = temp/16;	mod = temp - mok*16;	temp = mok;
		buff_s[6] = itoa(mod);

		sendto( client_socket, buff_s, BUFF_MAX, 0, ( struct sockaddr*)&server_addr, sizeof( server_addr));
	}      
   }

   server_addr_size  = sizeof( server_addr);
   recvfrom( client_socket, buff_rcv, BUFF_SIZE, 0 , 
            ( struct sockaddr*)&server_addr, &server_addr_size);
   printf( "receive: %s\n", buff_rcv);
   close( client_socket);
   
   return 0;
}

std::string frameTostring(cv::Mat frame)
{
	cv::Size size;
	std::vector<uchar> data;
	int imageSize_Total;

	size = frame.size();
	imageSize_Total = size.width * size.height * frame.channels();
	data.assign((uchar*)frame.datastart, (uchar*)frame.dataend);
	//std::vector<uchar> data(frame.ptr(), frame.ptr() + imageSize_Total);
	std::string image_String(data.begin(), data.end());
	return image_String;		
}

char itoa(int num)
{
	switch(num){
	case 0:
		return '0';
		break;
	case 1:
		return '1';
		break;
	case 2:
		return '2';
		break;
	case 3:
		return '3';
		break;
	case 4:
		return '4';
		break;
	case 5:
		return '5';
		break;
	case 6:
		return '6';
		break;
	case 7:
		return '7';
		break;
	case 8:
		return '8';
		break;
	case 9:
		return '9';
		break;
	case 10:
		return 'A';
		break;
	case 11:
		return 'B';
		break;
	case 12:
		return 'C';
		break;
	case 13:
		return 'D';
		break;
	case 14:
		return 'E';
		break;
	case 15:
		return 'F';
		break;
	default:
		return 'X';
	}
	return 0;
}

int atoi_(char c)
{
	switch(c){
	case '0':
		return 0;
	case '1':
		return 1;
	case '2':
		return 2;
	case '3':
		return 3;
	case '4':
		return 4;
	case '5':
		return 5;
	case '6':
		return 6;
	case '7':
		return 7;
	case '8':
		return 8;
	case '9':
		return 9;
	case 'A':
		return 10;
	case 'B':
		return 11;
	case 'C':
		return 12;
	case 'D':
		return 13;
	case 'E':
		return 14;
	case 'F':
		return 15;
	}
}
