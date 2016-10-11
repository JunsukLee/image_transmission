#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <vector>
#include <iostream>
#include "opencv2/opencv.hpp"

//#define  BUFF_SIZE   1024
#define  BUFF_SIZE   65536
#define  PORT        2223

using namespace std;
using namespace cv;

void receive_data_copy(char receive_buffer[], int receive_size);
bool check_data(bool count_bool[], int total_count);
cv::Mat stringToMat(std::string str);
char itoa(int num);
int  atoi_(char c);

std::string str;

int   main( void)
{
   int   server_data_sock;
   int   addrsize_data;

   int   count_receive;
   bool  count_bool[255];

   int   total_count;
   int   count_idx;
   int   receive_size;
   bool  collection = false;

   memset(count_bool, false, 255);

   struct sockaddr_in   server_data_addr;
   struct sockaddr_in   client_addr;

   char   buff_rcv[BUFF_SIZE];
   char   buff_snd[BUFF_SIZE];

   struct timeval tv;
   tv.tv_sec  = 0;
   tv.tv_usec = 100;

   server_data_sock  = socket( PF_INET, SOCK_DGRAM, 0);
   if( -1 == server_data_sock)
   {
      printf( "socket create fail\n");
      exit( 1);
   }

   if(setsockopt(server_data_sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0){
	printf( "socket setting fail\n");
	return 0;
   }
   
   memset( &server_data_addr, 0, sizeof( server_data_addr));
   server_data_addr.sin_family     = AF_INET;
   server_data_addr.sin_port       = htons( PORT);
   server_data_addr.sin_addr.s_addr= htonl( INADDR_ANY);

   if( -1 == bind( server_data_sock, (struct sockaddr*)&server_data_addr, sizeof( server_data_addr) ) )
   {
      printf( "bind() execute error\n");
      exit( 1);
   }

   while( 1)
   {
      addrsize_data  = sizeof( client_addr);       
      recvfrom( server_data_sock, buff_rcv, BUFF_SIZE, 0 , ( struct sockaddr*)&client_addr, &addrsize_data);      
      if(buff_rcv[0]=='D'){
	      //printf( "receive: %s\n", buff_rcv);
	      total_count = atoi_(buff_rcv[2])*10 + atoi_(buff_rcv[3]);
	      count_idx   = atoi_(buff_rcv[4])*10 + atoi_(buff_rcv[5]);
	      receive_size= atoi_(buff_rcv[6])*16*16*16 + atoi_(buff_rcv[7])*16*16 + atoi_(buff_rcv[8])*16 + atoi_(buff_rcv[9]);

	      printf( "receive total: %d\n", total_count);
	      printf( "receive id   : %d\n", count_idx);
	      printf( "receive_size : %d\n", receive_size);
	      printf( "len : %ld\n", sizeof(buff_rcv)/sizeof(buff_rcv[0]));

	      if(count_bool[count_idx] == false){
			count_bool[count_idx] = true;
			receive_data_copy(buff_rcv, receive_size);
	      }
	      if(check_data(count_bool, total_count)){ // finish data
			// decoding
			printf("str len : %ld\n", str.length());
			stringToMat(str);
			// init
			str.erase(str.begin(), str.end());
			memset(count_bool, false, 255);
	      }
      }
      memset(buff_rcv, 0, BUFF_SIZE);
      
      //sprintf( buff_snd, "%s%s", buff_rcv, buff_rcv);
      //sendto( server_data_sock, buff_snd, strlen( buff_snd)+1, 0,
      //               ( struct sockaddr*)&client_addr, sizeof( client_addr)); 
   }
}

void receive_data_copy(char receive_buffer[], int receive_size)
{
	for(int i=0; i<receive_size; i++)
		str += receive_buffer[i+10];
}

bool check_data(bool count_bool[], int total_count)
{
	for(int i=0; i < total_count ; i++){
		if(!count_bool[i]){
			return false;
			break;
		}
	}
	return true;
}

cv::Mat stringToMat(std::string str)
{
	/*std::vector<uchar> in(str.begin(), str.end());
	cv::Mat data_mat(in, true);
	cv::Mat image(cv::imdecode(data_mat, 1)); // put 0 if you want grayscale*/
	cv::Mat image(480, 640, CV_8UC3, str.data());
	cout << "Height: " << image.rows << " Width: " << image.cols << std::endl;
	int a;
	//while(1)
	{
		cv::namedWindow("SERVER_Video", CV_WINDOW_AUTOSIZE);
		imshow("SERVER_Video", image);
		std::cin >> a;
		if(a == 0)
			break;
	}

	//std::copy(str.begin(), str.end(), std::back_inserter(in));	
	//memcpy(out.data, in.data(), in.size()*sizeof(uchar));
	//printf("vector::data size : %ld\n", in.size());

	return image;
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
