#include "opencv2/opencv.hpp"
#include <iostream>
#include <string>
#include <vector>


using namespace cv;

int main()
{
    char *path ="copy.jpg";
//    char *buffera;
//    buffera = (char *)malloc(sizeof(char)*1024);

    cv::Mat mat = cv::imread(path);
    cv::Size size = mat.size();
    
    int total = size.width * size.height * mat.channels();
    std::cout << "Mat size = " << total << std::endl;
    
    std::vector<uchar> data(mat.ptr(), mat.ptr() + total);
    std::string s(data.begin(), data.end());
    std::cout << "String size = " << s.length() << std::endl;
    s.insert(0, "K");
    std::cout << "String size = " << s.length() << std::endl;

    std::cout << mat << std::endl;

    char buffer[1024];
    int  c=0;
    for(int i=0; i<s.length(); ){
	memset(buffer, 0, 1024);
	c++;
	for(int j=0; j<1024; j++, i++){
		if(i>= s.length())
			break;		
		buffer[j] = s.at(i);				
		if(i==0)
			printf("%c\n", buffer[0]);
	}

    }
    printf("C : %d\n", c);
    printf("%c\n", buffer[0]);
    printf("%c\n", buffer[1]);
    printf("%c\n", buffer[2]);
    printf("%c\n", buffer[3]);
    printf("%c\n", buffer[4]);

    //for(int j=0; j<1024; j++){
	//std::cout << buffer << "\n" << std::endl;
    //}


    /*VideoCapture cap;
    cap.open(0);
    if(!cap.isOpened())
        return -1;

    cv::Mat frame;
    //nameWindow("edges", 1);
    while(1)
    {
        cap >> frame;
        imshow("edges", frame);
        if(waitKey(30) >= 0)
            break;
	imwrite("copy.jpg", frame);
    }*/
    return 0;
}

