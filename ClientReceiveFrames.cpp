#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <stdlib.h>	/* malloc, free, rand */

#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
//#include <stdio.h>
#include <string.h>
//#include <stdlib.h>	//exit()

#pragma comment(lib, "Ws2_32.lib")



#define DEFAULTPORT "27015"
//#define DEFAULT_BUFLEN 512
#define DEFAULT_BUFLEN 614400  //921654	//614400
#define DEFAULT_HOST_IP	"192.168.88.107"		// 2.170"

void fwriteData(FILE* fp,unsigned char* data, int width, int height, int nChannels, int padding){
	int n;
	unsigned char *bytes = data;
	int BytesPerRow = width*nChannels  + padding;	//	chia het cho 4
	int BytesSize = BytesPerRow * height;
	if(!fp){
		perror("ERROR: file not founded\n");
	}

	for(n = BytesPerRow; n <= BytesSize; n+=BytesPerRow){	//anh bmp//////////////////////////////////////////////
		fwrite(bytes+BytesSize-n,1,BytesPerRow,fp);
	}

}

typedef struct _BITMAP4{
	unsigned char r;    //1 byte * 4 = 4 byte = 1 int
	unsigned char g;
	unsigned char b;
	//unsigned char a;
} BITMAP4;

BITMAP4 YUV_to_Bitmap(int y,int u,int v)
{
	int r,g,b;
	BITMAP4 bm = {0,0,0};

	// u and v are +-0.5
	u -= 128;
	v -= 128;

	// Conversion
	r = y + 1.370705 * v;
	g = y - 0.698001 * v - 0.337633 * u;
	b = y + 1.732446 * u;

	// Clamp to 0..1
	if (r < 0) r = 0;
	if (g < 0) g = 0;
	if (b < 0) b = 0;
	if (r > 255) r = 255;
	if (g > 255) g = 255;
	if (b > 255) b = 255;
	///V4L2_PIX_FMT_RGB24   ,   V4L2_PIX_FMT_BGR24
	bm.r = (unsigned char)r;
	bm.g = (unsigned char)g;
	bm.b = (unsigned char)b;
	//bm.a = 0;

	return(bm);
}

void yuyv2rgb(int* yuyv_2_pixels, BITMAP4* rgb1, BITMAP4* rgb2){
	int y1,y2,u,v;

	//http://v4l.videotechnology.com/dwg/v4l2.html#V4L2-PIX-FMT-YUYV
	y1  = ((*yuyv_2_pixels & 0x000000ff));
	u  = ((*yuyv_2_pixels & 0x0000ff00)>>8);
	y2  = ((*yuyv_2_pixels & 0x00ff0000)>>16);
	v = ((*yuyv_2_pixels & 0xff000000)>>24);
	//yuv to rgb
	*rgb1 = YUV_to_Bitmap(y1,u,v);
	*rgb2 = YUV_to_Bitmap(y2,u,v);
}

void cvtYUYV2RGBImage(int* buffer, int width, int height, int nChannels, unsigned char* output){
	size_t sizeOfBuffer = width*height*nChannels;	//byte
	int *yuyvpixels = buffer;    //get 2 pixels = 4 bytes
	unsigned char *buffer_char = output;    //1 byte

	int i = 0;
	int numOfPixels =  width*height/2;
	int i_rgb = 0;
	for(i = 0; i< numOfPixels ; i++){

		BITMAP4 rgb1,rgb2;
		yuyv2rgb(&yuyvpixels[i],&rgb1,&rgb2);	//6bytes = 2pixels rgb

     	buffer_char[i_rgb]   = rgb1.b;
		buffer_char[i_rgb+1] = rgb1.g;
		buffer_char[i_rgb+2] = rgb1.r;

		buffer_char[i_rgb+3] = rgb2.b;
		buffer_char[i_rgb+4] = rgb2.g;
		buffer_char[i_rgb+5] = rgb2.r;

		i_rgb +=6;

	}

}

#define SIZE_YUYV 614400

int recvExtn(SOCKET socket, char* buffer, int nBytes){
	int n = 0;
	do {
		n = recv(socket, buffer, nBytes, 0);		//default flags = 0		
		if (n == 0){
			return 0;		//Connection closed
		}	
		if (n < 0){
			return -1;		// receive failed
		}		
	} while (n != nBytes);	
	return n;
}

int onReceive(SOCKET socket, char* buffer, FILE* filebuffer)
{
	int BytesPerRow = 4;//512;//1280;//640*2	//1920;	//640*3
	int nBytes = 0;
	int iResult = 0;
	do{
		iResult = recvExtn(socket, buffer,BytesPerRow);
		if (iResult > 0) {
			nBytes +=iResult;
			fwrite(buffer, 1, BytesPerRow, filebuffer);
		}
		else if (iResult == 0)
			printf("Connection closed\n");
		else
			printf("recv failed: %d\n", WSAGetLastError());
	} while (iResult > 0);

	return nBytes;
}

int main(int argc, char* argv[]) {
	WSADATA wsaData;
	int iResult;
	argv[1] = DEFAULT_HOST_IP;
	//// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed: %d\n", iResult);
		return 1;
	}
	//create address infomation
	struct addrinfo *result = NULL,	//dst address information to store
					*ptr = NULL,	
					hints;			//address information to set

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;	// AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	//request the IP address for the server name passed on the cmd
	// TCP port on the server that the client will connect to is DEFAULTPORT
	printf("DEFAULT PORT : %s\n",	
		DEFAULTPORT);
	iResult = getaddrinfo(argv[1], DEFAULTPORT, &hints, &result);// [server infomation] IP: argv[1], port : DEFAULTPORT
	if (iResult != 0) {
		printf("getaddrinfo failted: %d\n", iResult);
		return 1;
	}

	//create SOCKET
	SOCKET ConnectSocket = INVALID_SOCKET;
	ptr = result;
	// server socket
	printf("client : Create a SOCKET for connecting to server\n");
	ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
	if (ConnectSocket == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	printf("Connect to server.\n");
	iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen); //ai_addr : IP + port of server
	if (iResult == SOCKET_ERROR) {
		closesocket(ConnectSocket);
		ConnectSocket = INVALID_SOCKET;
	}
	// should really try the next address returned by getaddrinfo
	// in the example we just free it
	freeaddrinfo(result);
	if (ConnectSocket == INVALID_SOCKET) {
		printf("Unable to connect to server!\n");
		WSACleanup();
		return 1;
	}

	int recvbuflen = DEFAULT_BUFLEN;
	char recvbuf[DEFAULT_BUFLEN];	//storage to receive
	


	// receive data until the server closes the conncetion
	/*
	int count = 0, sum = 0;
	FILE *filebuffer = fopen("buffer","w+b");	//write update binary

	int BytesPerRow = 4;//512;//1280;//640*2	//1920;	//640*3

	iResult = onReceive(ConnectSocket, recvbuf, filebuffer);
	fclose(filebuffer);
	do{
		iResult = recvExtn(ConnectSocket, recvbuf,BytesPerRow);
		if (iResult > 0) {
			sum +=iResult;
			fwrite(recvbuf,1,BytesPerRow,filebuffer);
		}
		else if (iResult == 0)
			printf("Connection closed\n");
		else
			printf("recv failed: %d\n", WSAGetLastError());
	} while (iResult > 0);
	
	printf("sizeof recvbuf: %d, bytes received: %d\n",sizeof(recvbuf),iResult);
	
	memset(recvbuf,0,recvbuflen);

	//CONVERT YUYV TO BITMAP
	FILE* yuyvfile = fopen("buffer","rb");
	FILE *bmpfile = fopen("image.bmp","wb");
	char* new_buffer = (char*)malloc(921600);
	memset(new_buffer,0,921600);
	unsigned int width = 640;
	unsigned int height = 480;
	int nChannels = 3;
	int size = width*height*2;

	fread(recvbuf,1,recvbuflen,yuyvfile);
	cvtYUYV2RGBImage((int*)recvbuf, width, height, nChannels,(unsigned char*) new_buffer);
	createBMPfile("image.bmp",(unsigned char*)new_buffer,width,height,nChannels,0);
	fclose(yuyvfile);
	fclose(bmpfile);
	//////////////////////////
	*/
	int n = 0;
	char msg[20];
	int i = 0;
	snprintf(msg, 20, "message%d", i);
	i++;
	n = send(ConnectSocket, msg, 20, 0);
	printf("send %d bytes\n", n);
	n = recv(ConnectSocket, msg, 20, 0);
	printf("recv %s, %d bytes\n",msg, n);

	snprintf(msg, 20, "message%d", i);
	i++;
	n = send(ConnectSocket, msg, 20, 0);
	printf("send %d bytes\n", n);
	n = recv(ConnectSocket, msg, 20, 0);
	printf("recv %s, %d bytes\n", msg, n);

	snprintf(msg, 20, "message%d", i);
	i++;
	n = send(ConnectSocket, msg, 20, 0);
	printf("send %d bytes\n", n);
	n = recv(ConnectSocket, msg, 20, 0);
	printf("recv %s, %d bytes\n", msg, n);

	snprintf(msg, 20, "message%d", i);
	i++;
	n = send(ConnectSocket, msg, 20, 0);
	printf("send %d bytes\n", n);
	n = recv(ConnectSocket, msg, 20, 0);
	printf("recv %s, %d bytes\n", msg, n);

	do {
		snprintf(msg, 20, "message%d", i);
		i++;
		n = send(ConnectSocket, msg, 20, 0);
		printf("send %d bytes\n", n);
		n = recv(ConnectSocket, msg, 20, 0);
		printf("recv %s , %d bytes\n",msg, n);

	} while (i < 10000);



	printf("press enter");
	getchar();
	return 0;
}
