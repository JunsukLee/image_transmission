#include "Transfer_image.hpp"

Transfer_image::Transfer_image()
{
}

Transfer_image::~Transfer_image()
{
	socket_close();
}

void Transfer_image::init()
{
	cap.open(0);
}

void Transfer_image::start_video()
{
	if(!cap.isOpened()){
		printf("Connecting Video Cam is Failed!!\n");
        	return;
	}
	cv::namedWindow("Video", CV_WINDOW_AUTOSIZE);
	cv::Mat frame;
	while(1)
	{
		cap >> frame;
		get_currentFrame(frame);
		imshow("Video", frame);
		if(cv::waitKey(120) >= 0)
			break;		
	}
}


void Transfer_image::get_currentFrame(cv::Mat frame)
{
	if(status_transmission){
		size = frame.size();
		imageSize_Total = size.width * size.height * frame.channels();
		std::vector<uchar> data;
		data.assign((uchar*)frame.datastart, (uchar*)frame.dataend);
		//std::vector<uchar> data(frame.ptr(), frame.ptr() + imageSize_Total);
		std::string image_String(data.begin(), data.end());
		socket_transmission(image_String);		
	}
}

void Transfer_image::socket_connected()
{
	client_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
	ready = true;
	
	if(connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr))) {
		printf("Connect Failed\n");
		return ;
	}
	printf("Connected!\n");
}

void Transfer_image::socket_transmission(std::string image_String) // total00 id00 size0000
{	
	if(!ready){
		return;
	}
	
	const int BUFFERSIZE = 1024;
	//const int BUFFERSIZE = 2048;
	//const int BUFFERSIZE = 4096;
	

	char buffer_data[BUFFERSIZE];
	int segment_Count = 0;
	int segment_Total_Count;

	// calc :: segment_Total_Count
	segment_Total_Count = image_String.length() / (BUFFERSIZE-10) - 1;
	((image_String.length() % (BUFFERSIZE-10)) == 0) ? : segment_Total_Count++;

	// 1st sending	
	for(int i=0, j=0; i < image_String.length(); )
	{
		memset(buffer_data, 0, BUFFERSIZE);
		segment_Count++;
		
		//setting 1 packet
		for(j=10; j<BUFFERSIZE; j++, i++)
		{
			if(i >= image_String.length())
				break;
			buffer_data[j] = image_String.at(i);
		}

		if(segment_Count <= segment_Total_Count){
			//ADD total
			buffer_data[2] = itoa((int)(segment_Total_Count%16));
			buffer_data[1] = itoa((int)(segment_Total_Count/16)%16);
			buffer_data[0] = itoa((int)(segment_Total_Count/16)/16);
			
			//ADD id
			buffer_data[5] = itoa((int)(segment_Count%16));
			buffer_data[4] = itoa((int)(segment_Count/16)%16);
			buffer_data[3] = itoa((int)(segment_Count/16)/16);

			//ADD size
			buffer_data[6] = itoa((int)((j)/1000));
			buffer_data[7] = itoa((int)(((j)%1000)/100));
			buffer_data[8] = itoa((int)(((j)%100)/10));
			buffer_data[9] = itoa((int)((j)%10));

			for(int k=9; k>=0; k--){
				if(buffer_data[k]=='X'){
					//i-= (j-10);
					break;
				}
				if(k==0){
					//Write
					write(client_socket, buffer_data, j);
					
					printf("data total size : %ld\n", (long)image_String.length());
					printf("segment_Total_Count : %d\n", segment_Total_Count);
					printf("segment_Count :       %d\n", segment_Count);
				}					
			}
		}

	}

	// 2st sending
	int ask;
	while(1){
		ask = socket_receive();
		if(ask == 1 || ask == 2){ break; }
		ask = ask / 10;
		ask = ask - 1;
		memset(buffer_data, 0, BUFFERSIZE);
		int i;
		for(i=10; i < BUFFERSIZE; i++){
			if((ask*(BUFFERSIZE-10)+(i-10)) >= image_String.length())
				break;
			buffer_data[i] = image_String.at(ask*(BUFFERSIZE-10)+(i-10));
		}

		resetting:

		//ADD total
		buffer_data[2] = itoa((int)(segment_Total_Count%16));
		buffer_data[1] = itoa((int)(segment_Total_Count/16)%16);
		buffer_data[0] = itoa((int)(segment_Total_Count/16)/16);
		
		//ADD id
		buffer_data[5] = itoa((int)((ask+1)%16));
		buffer_data[4] = itoa((int)((ask+1)/16)%16);
		buffer_data[3] = itoa((int)((ask+1)/16)/16);

		//ADD size
		buffer_data[6] = itoa((int)((i)/1000));
		buffer_data[7] = itoa((int)(((i)%1000)/100));
		buffer_data[8] = itoa((int)(((i)%100)/10));
		buffer_data[9] = itoa((int)((i)%10));

		for(int k=9; k>=0; k--){
			if(buffer_data[k]=='X'){
				goto resetting;
				break;
			}
			if(k==0){
				//Write
				write(client_socket, buffer_data, i);
			
				printf("E_data total size : %ld\n", (long)image_String.length());
				printf("E_segment_Total_Count : %d\n", segment_Total_Count);
				printf("E_segment_Count :       %d\n", segment_Count);
			}					
		}
	}
}

int Transfer_image::socket_receive()
{
	char buffer_receive[10];
	int len;
	while(1)
        {
		memset(buffer_receive,0,10);
		len=read(client_socket, buffer_receive, 10);
		//printf("len : %d\n", len);

                if(len<1){
			return 0;
		}else if(len>=1){
			printf("client : %s\n", buffer_receive);			
			if(!strncmp(buffer_receive,"ack", 3)){
				printf("ACK in \n");
				ready = true;
				return 1;
			}
			if(!strncmp(buffer_receive,"neg", 3)){
				printf("NEG in \n");
				usleep(10000);
				neg_status_recovery();
				ready = true;				
				return 2;
			}
			if(buffer_receive[0] == 'e'){
				std::cout << "error in : " << buffer_receive << std::endl;
				int err_index;
				err_index = charToInt(buffer_receive[1])*100;
				err_index += charToInt(buffer_receive[2])*10;
				err_index += charToInt(buffer_receive[3]);
				ready = false;

				//usleep(10000);				
				return err_index*10;
			}

		}
         }

	return 0;
}

void Transfer_image::neg_status_recovery()
{
	set_off_transmission();
	socket_close();
	video_close();
	usleep(3000000);

	//set_on_transmission();
	status_transmission = true;
	socket_connected();
	start_video();
	//usleep(10000);
}


void Transfer_image::socket_close()
{
	close(client_socket);	
}

void Transfer_image::video_close()
{
	cv::destroyWindow("Video");
}

bool Transfer_image::set_on_transmission()
{
	status_transmission = true;
	return status_transmission;
}

bool Transfer_image::set_off_transmission()
{
	status_transmission = false;
	return status_transmission;
}

int Transfer_image::charToInt(char c)
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

char Transfer_image::itoa(int num)
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




