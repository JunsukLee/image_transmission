#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include <vector>
#include "opencv2/opencv.hpp"

#include <curl/curl.h>
#include <time.h>

using namespace std;
using namespace cv;

void *text_re(void *arg);
std::string char_double_pointer_To_string(char **data, int row);
cv::Mat stringToMat(std::string str);
int charToInt(char c);

int client_count = 0;
int client_sock[100];

// sudo g++ server.cpp -o server -lpthread -std=c++11 -fpermissive

int main()
{
	int server_sock;
	int option=1, addrsize, len, port=2223;
	int i;
	struct sockaddr_in server_addr, client_addr;
	struct linger ling;
	
	ling.l_onoff  = 1;
	ling.l_linger = 0;

	pthread_t th_send[100];
	pthread_t th_receive[100];

	server_sock= socket(PF_INET, SOCK_STREAM,IPPROTO_TCP);
	setsockopt(server_sock, SOL_SOCKET, SO_LINGER, (char *)&ling, (int)sizeof(ling));	
	//setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, (char *)&option, (int)sizeof(option));
	
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(port);

	if(bind(server_sock,(struct sockaddr *)&server_addr, sizeof(server_addr)))
		return 0;
	listen(server_sock,10);

	while(client_count < 100){		
		addrsize = sizeof(client_addr);
		client_sock[client_count] = accept(server_sock, (struct sockaddr *)&client_addr, &addrsize);

		if(client_sock[client_count] != -1){
			printf("NEW \n");
			printf("Hello client %d\n", client_count);
			pthread_create(&th_receive[client_count],NULL, &text_re, &client_sock[client_count]);
			pthread_join(&th_receive[client_count],NULL);

			printf("\npthread join\n\n");
			client_count++;
		}
		sleep(2);
	}

	for(i=0; i < client_count ; i++)
		close(client_sock[i]);

	return 0;
}

void *text_re(void *arg)
{
	
        int *sock=(int*)arg;
        char buffer[1024];
        int len;
	int for_i=0;

	long segment_total_count;
	long segment_count = 0;
	int count_temp;
	int segment_size;
	bool front_data_check;

	cv::Mat final_mat;

	char **receive_buffer = new char*[1000];
	for(int i=0; i<1000; i++){
		receive_buffer[i] = new char[1014];
		memset(receive_buffer[i], 0, 1014);
	}
	

        while(1)
        {
		memset(buffer,0,1024);
		len=read((int)*sock,buffer,1024);
		//printf("\nlen : %d\n", len);
		//std::cout << "tol : " << buffer[0] << " " << buffer[1] << " " << buffer[2] << std::endl;
		//std::cout << "cnt : " << buffer[3] << " " << buffer[4] << " " << buffer[5] << std::endl;
		//printf("tol : %c %c %c\n", buffer[0], buffer[1], buffer[2]);
		//printf("cnt : %c %c %c\n", buffer[3], buffer[4], buffer[5]);

                if(len<1){
                        break;
		}else if(len>=1){
			//printf("client : %s\n", buffer);
			//get Total_count
			segment_total_count = charToInt(buffer[0])*256 + charToInt(buffer[1])*16 + charToInt(buffer[2]);
			//printf("tol : %ld\n", segment_total_count);

			//get current_count
			segment_count++;
			count_temp = charToInt(buffer[3])*256 + charToInt(buffer[4])*16 + charToInt(buffer[5]);
			//printf("cnt : %ld\n", count_temp);

			//printf("segment_count : %ld\n", segment_count);
	
			//get segment_size
			segment_size = charToInt(buffer[6]) * 1000 + charToInt(buffer[7]) * 100 
					 + charToInt(buffer[8]) * 10 + charToInt(buffer[9]);
			//printf("segment_size : %ld\n", segment_size);

			//front_data_check
			front_data_check = false;
			//for(int i=9; i>=0; i--){
			for(int i=0; i<=9; i++){
				int num = charToInt(buffer[i]);
				//printf("%d_num : %d\n", i, num);

				if(num >= 0 && num <= 15){
					front_data_check = true;
				}else{
					front_data_check = false;
					printf("front_data_check failed\n");

					break;
				}
			}
			if(!front_data_check){
				write(*sock, "neg", 4);
				printf("neg\n");
				segment_count = 0;
				segment_total_count = 0;
				for(int i=0; i<1000; i++){ // init
					memset(receive_buffer[i], 0, 1014);
				}
			}

			//main_data_copy
			for(int i=0; i < 1014 && front_data_check; i++){
				receive_buffer[count_temp-1][i] = buffer[i+10];
			}

			if((segment_count == segment_total_count) && front_data_check){
				char ack_buffer[] = "ack";
				write(*sock, ack_buffer, 4);
				printf("ack\n");
				
				stringToMat(char_double_pointer_To_string(receive_buffer, segment_total_count));
				for(int i=0; i<segment_total_count; i++){				
					memset(receive_buffer[i], 0, 1014);
				}
				segment_count = 0;
				segment_total_count = 0;
			}
		}
         }
}

std::string char_double_pointer_To_string(char **data, int row)
{
	string str;
	for(int i=0; i<row; i++){
		str += data[i];
	}
	return str;
}

cv::Mat stringToMat(std::string str)
{
	std::vector<uchar> in;
	cv::Mat out;

	std::copy(str.begin(), str.end(), std::back_inserter(in));	

	memcpy(out.data, in.data(), in.size()*sizeof(uchar));
 


	printf("vector::data size : %ld\n", in.size());

	return out;
}

int charToInt(char c)
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




