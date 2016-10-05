#include <stdio.h>
#include <iostream>
#include "Transfer_image.hpp"

// sudo g++ cclient.cpp Transfer_image.cpp -o cclient `pkg-config --libs --cflags opencv`

int main()
{
	Transfer_image transfer_image;

	transfer_image.set_on_transmission();
	transfer_image.init();
	transfer_image.socket_connected();
	transfer_image.start_video();

		

	return 0;
}
