#include <stdio.h>
#include <string.h>
#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#define SERVER_PORT 2223
#define SERVER_IP   "127.0.0.1"
//#define SERVER_IP   "203.230.103.47"

using namespace std;

void *receive(void *args);
void *send_image(void *args);

int main()
{
	int	client_socket;
	struct	sockaddr_in server_addr;
	int	len;
	char	buffer[BUFSIZ];
	int	n;

	pthread_t th_receive;
	pthread_t th_send_image;

	client_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

	if(connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr))) {
		printf("Connect Failed\n");
		return 0;
	}
	printf("Connected!\n");

	pthread_create(&th_receive, NULL, &receive, (void *)&client_socket);
	printf("1\n");
	pthread_create(&th_send_image, NULL, &send_image, (void *)&client_socket);
	printf("2\n");

	pthread_join(th_receive, NULL);
	pthread_join(th_send_image, NULL);

	while(1) {
		sleep(20);
	}

	close(client_socket);	

	return 0;
}

void *receive(void *args){
	int *client_sock = (int *)args;
	int len;
	char buffer_data[1024];
//	char *buffer_data;
//	buffer_data = (char *)malloc(sizeof(char)*1024);

	while(1) {
		memset(buffer_data, 0, 1024);
		len = read(*client_sock, buffer_data, 1024);

		//if(len < 1) break;
		printf("Received: %s\n", buffer_data);
	}
}

void *send_image(void *args){
         int *sock=(int*)args;
         char buffer_data[1024];
         while(1){
		printf("client : ");
		scanf("%s", buffer_data);
		write(*sock,buffer_data,1024);
		memset(buffer_data, 0, 1024);
         }
}
