/*
 ============================================================================
 Name        : yuyv2rgb.c
 Author      :
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

/*
 * https://jwhsmith.net/2014/12/capturing-a-webcam-stream-using-v4l2/
 */

#include <sys/ioctl.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/videodev2.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>	//open() close() read() write()
#include <stdlib.h>	//exit()


void fwriteData(FILE* fp,void* data, int width, int height, int nChannels, int padding){
	int n;
	unsigned char *bytes = data;
	int BytesPerRow = width*nChannels  + padding;	//	chia het cho 4
	int BytesSize = BytesPerRow * height;
	if(!fp){
		printf("file not founded");
	}


//	for(n = 0; n < BytesSize; n+=BytesPerRow){	//bgr
//		fwrite(bytes+n,1,BytesPerRow,fp);
//	}

	for(n = BytesPerRow; n <= BytesSize; n+=BytesPerRow){	//anh bmp//////////////////////////////////////////////
		fwrite(bytes+BytesSize-n,1,BytesPerRow,fp);
	}

}

// unsigned char : 1byte, short int 2 bytes, int 4 bytes, long int 8 bytes;	for Intel
int createBMPfile(char* filename, void* data, int width, int height, int nChannels, int padding){
	unsigned short int word[1]; /* 2 bytes */
	unsigned int dword[1]; 		/* 4 bytes */

	unsigned int	Width= width,	Height=height;
				   /* in bytes */
	unsigned int   FileHeaderSize=14,
				   InfoHeaderSize=40, // header_bytes
				   BytesPerRow,
				   FileSize,
				   OffsetBits,
				   BytesSize; // full size of the file including the header, the pixel data, and all padding

	//PRINT
	BytesPerRow = Width*nChannels  + padding;	//(((Width * bpp)+31)/32)*4;	chia het cho 4
	printf("BytesPerRow= %d\n", BytesPerRow);
	BytesSize = BytesPerRow*Height;	//kich thuoc data anh tinh theo byte
	printf("BytesSize= %d\n", BytesSize);
	FileSize = FileHeaderSize+InfoHeaderSize+BytesSize;	//+PaletteSize;
	printf("FileSize= %d\n", FileSize);
	OffsetBits = FileHeaderSize+InfoHeaderSize;//+PaletteSize;
	printf("OffsetBits= %d\n", OffsetBits);

	//--------------------------

	FILE *fp = fopen(filename, "wb"); /* b - binary mode */
	/* bmp file header */
	word[0]=19778;                                         fwrite(word,1,2,fp); /* file Type signature = BM */
	dword[0]=FileSize;                                     fwrite(dword,1,4,fp); /* FileSize */
	word[0]=0;                                             fwrite(word,1,2,fp); /* reserved1 */
	word[0]=0;                                             fwrite(word,1,2,fp); /* reserved2 */
	dword[0]=OffsetBits;                                   fwrite(dword,1,4,fp); /* OffsetBits */
	dword[0]=InfoHeaderSize;                               fwrite(dword,1,4,fp);
	dword[0]=Width;                                        fwrite(dword,1,4,fp);
	dword[0]=Height;                                       fwrite(dword,1,4,fp);
	word[0]=1;                                             fwrite(word,1,2,fp); /* planes */	//The number of color planes, must be set to 1
	word[0]=24;                                            fwrite(word,1,2,fp); /* Bits of color per pixel */ //The number of bits per pixel. For an RGB image with a single byte for each color channel the value would be 24
	dword[0]=0;                                            fwrite(dword,1,4,fp); /* compression type */	// 0: no compression
	dword[0]=BytesSize;                                    fwrite(dword,1,4,fp); /* Image Data Size, set to 0 when no compression */
	dword[0]=2835;                                         fwrite(dword,1,4,fp); /* This is the horizontal resolution */
	dword[0]=2835;                                         fwrite(dword,1,4,fp); /* This is the vertical resolution */
	dword[0]=0;                                            fwrite(dword,1,4,fp); /*  number of used colors, default:0 all colors*/
	dword[0]=0;                                            fwrite(dword,1,4,fp); /* The important colors, leave at 0 to default to all colors */

	fwriteData(fp,data,Width,Height,nChannels,padding);

	fclose(fp);
	printf("file saved %s\n",filename);
	return 0;
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
	BITMAP4 bm = {0,0,0,0};

	// u and v are +-0.5
	u -= 128;
	v -= 128;

	// Conversion
	r = y + 1.370705 * v;
	g = y - 0.698001 * v - 0.337633 * u;
	b = y + 1.732446 * u;

	/*
   r = y + 1.402 * v;
   g = y - 0.344 * u - 0.714 * v;
   b = y + 1.772 * u;
	 */
	/*
   y -= 16;
   r = 1.164 * y + 1.596 * v;
   g = 1.164 * y - 0.392 * u - 0.813 * v;
   b = 1.164 * y + 2.017 * u;
	 */

	// Clamp to 0..1
	if (r < 0) r = 0;
	if (g < 0) g = 0;
	if (b < 0) b = 0;
	if (r > 255) r = 255;
	if (g > 255) g = 255;
	if (b > 255) b = 255;
	///V4L2_PIX_FMT_RGB24   ,   V4L2_PIX_FMT_BGR24
	bm.r = r;
	bm.g = g;
	bm.b = b;
	//bm.a = 0;

	return(bm);
}

void yuyv2rgb(int* yuyv_2_pixels, BITMAP4* rgb1, BITMAP4* rgb2){
	int y1,y2,u,v;
	//char r,g,b;
	/*//https://social.msdn.microsoft.com/Forums/windowsdesktop/en-US/1071301e-74a2-4de4-be72-81c34604cde9/program-to-translate-yuyv-to-rgbrgb?forum=windowsdirectshowdevelopment
    v  = ((*yuyv_2_pixels & 0x000000ff));
    y1  = ((*yuyv_2_pixels & 0x0000ff00)>>8);
    u  = ((*yuyv_2_pixels & 0x00ff0000)>>16);
    y2 = ((*yuyv_2_pixels & 0xff000000)>>24);
	 */
	/*//http://paulbourke.net/dataformats/yuv/
    u  = ((*yuyv_2_pixels & 0x000000ff));
    y1  = ((*yuyv_2_pixels & 0x0000ff00)>>8);
    v  = ((*yuyv_2_pixels & 0x00ff0000)>>16);
    y2 = ((*yuyv_2_pixels & 0xff000000)>>24);
	 */
	//http://v4l.videotechnology.com/dwg/v4l2.html#V4L2-PIX-FMT-YUYV
	y1  = ((*yuyv_2_pixels & 0x000000ff));
	u  = ((*yuyv_2_pixels & 0x0000ff00)>>8);
	y2  = ((*yuyv_2_pixels & 0x00ff0000)>>16);
	v = ((*yuyv_2_pixels & 0xff000000)>>24);
	//yuv to rgb
	*rgb1 = YUV_to_Bitmap(y1,u,v);
	*rgb2 = YUV_to_Bitmap(y2,u,v);
}

void cvtYUYV2RGBImage(void* buffer, int width, int height, int nChannels, void* output){
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
/*
		buffer_char[i_rgb]   = rgb1.r;
		buffer_char[i_rgb+1] = rgb1.g;
		buffer_char[i_rgb+2] = rgb1.b;

		buffer_char[i_rgb+3] = rgb2.r;
		buffer_char[i_rgb+4] = rgb2.g;
		buffer_char[i_rgb+5] = rgb2.b;
		i_rgb +=6;
		*/
	}

}

void print_bufferinfo(struct v4l2_buffer *bufferinfo){
	printf("buffer_info\n"
		"bufferinfo.bytesused: %d\n"
		"bufferinfo.field: %d\n"
		"bufferinfo.flags: %d\n"
		"bufferinfo.index: %d\n"
		"bufferinfo.length: %d\n"
		"bufferinfo.memory: %d\n"
		"bufferinfo.type: %d\n",
		bufferinfo->bytesused,
		bufferinfo->field,
		bufferinfo->flags,
		bufferinfo->index,
		bufferinfo->length,
		bufferinfo->memory,
		bufferinfo->type);
}

int main(void){
	int fd;
	if((fd = open("/dev/video0", O_RDWR)) < 0){
		perror("open");
		return 1;//exit(1);
	}
	//Retrieve the device’s capabilities
	struct v4l2_capability cap;
	if(ioctl(fd, VIDIOC_QUERYCAP, &cap) < 0){
		perror("VIDIOC_QUERYCAP");
		return 1;//exit(1);
	}

	if(!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)){
		fprintf(stderr, "The device does not handle single-planar video capture.\n");
		return 1;//exit(1);
	}

	/* you’ll need to use VIDIOC_S_FMT (set format) to tell your device. This is done using a v4l2_format structure:*/
	struct v4l2_format format;
	format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	format.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;//V4L2_PIX_FMT_MJPEG;
	///format.fmt.pix.pixelformat = V4L2_PIX_FMT_BGR24;
	format.fmt.pix.width = 640;//800;
	format.fmt.pix.height = 480;//600;

	if(ioctl(fd, VIDIOC_S_FMT, &format) < 0){
		perror("VIDIOC_S_FMT");
		return 1;//exit(1);
	}
	//init mmap
	/*Inform the device about your future buffers*/
	struct v4l2_requestbuffers bufrequest;
	bufrequest.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	bufrequest.memory = V4L2_MEMORY_MMAP;
	bufrequest.count = 1;

	if(ioctl(fd, VIDIOC_REQBUFS, &bufrequest) < 0){
		perror("VIDIOC_REQBUFS");
		return 1;//exit(1);
	}

	/*Allocate your buffers*/
	struct v4l2_buffer bufferinfo1;	//another v4l2_buffer stru
	memset(&bufferinfo1, 0, sizeof(bufferinfo1));

	bufferinfo1.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	bufferinfo1.memory = V4L2_MEMORY_MMAP;
	bufferinfo1.index = 0;

	if(ioctl(fd, VIDIOC_QUERYBUF, &bufferinfo1) < 0){
		perror("VIDIOC_QUERYBUF");
		return 1;//exit(1);
	}

	void* buffer_start = mmap(
			NULL,
			bufferinfo1.length,
			PROT_READ | PROT_WRITE,
			MAP_SHARED,
			fd,
			bufferinfo1.m.offset
	);

	if(buffer_start == MAP_FAILED){
		perror("mmap");
		return 1;//exit(1);
	}

	memset(buffer_start, 0, bufferinfo1.length);

	/*Get a frame*/
	struct v4l2_buffer bufferinfo;
	memset(&bufferinfo, 0, sizeof(bufferinfo));

	bufferinfo.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	bufferinfo.memory = V4L2_MEMORY_MMAP;
	bufferinfo.index = 0; /* Queueing buffer index 0. */

	// Activate streaming
	int type = bufferinfo.type;
	if(ioctl(fd, VIDIOC_STREAMON, &type) < 0){
		perror("VIDIOC_STREAMON");
		return 1;//exit(1);
	}

	// Put the buffer in the incoming queue.
	if(ioctl(fd, VIDIOC_QBUF, &bufferinfo) < 0){
		perror("VIDIOC_QBUF");
		return 1;//exit(1);
	}

	// The buffer's waiting in the outgoing queue.
	if(ioctl(fd, VIDIOC_DQBUF, &bufferinfo) < 0){
		perror("VIDIOC_QBUF");
		return 1;//exit(1);
	}
	/*Save frame as an image*/
/*	int imagefile;
	if((imagefile  = open("myimage.bmp", O_WRONLY | O_CREAT, 0660)) < 0){
		//if((imagefile = open("myimage.bmp", O_WRONLY | O_CREAT, 0660)) < 0){
		perror("open");
		return 1;//exit(1);
	}
	//write(imagefile, buffer_start, bufferinfo.length);
	//close(imagefile);
	//printf("save myimage.bmp\n");
*/

	unsigned int width = format.fmt.pix.width;
	unsigned int height = format.fmt.pix.height;

	int nChannels = 3;
	size_t sizeOfBuffer = width*height*nChannels;
	void* new_buffer = malloc(sizeOfBuffer);

	cvtYUYV2RGBImage(buffer_start, width, height, nChannels, new_buffer);
	char* filename = "yuyv2rgb.bmp";
	createBMPfile(filename,new_buffer,width,height,nChannels,0);


//	BITMAP4 rgb1 ;//= YUV_to_Bitmap(0,0,0);
//	BITMAP4 rgb2 ;//= YUV_to_Bitmap(255,0,0);
//	int _4_byte = 0xff0000ff;
//	yuyv2rgb(&_4_byte,&rgb1,&rgb2);	//6bytes = 2pixels rgb
//	unsigned char pixel[] = {rgb1.b,rgb1.g,rgb1.r,rgb2.b,rgb2.g,rgb2.r,0,0};
//	printf("r,g,b %d,%d,%d\n",rgb1.r,rgb1.g,rgb1.b);
//	char* filename = "yuyv2rgb.bmp";
//	createBMPfile(filename,pixel,2,1,nChannels,2);


	/////////////////////////////////////////////////
	print_bufferinfo(&bufferinfo);
	/////////////////////////////////////////////////


	// Deactivate streaming
	if(ioctl(fd, VIDIOC_STREAMOFF, &type) < 0){
		perror("VIDIOC_STREAMOFF");
		return 1;//exit(1);
	}

	close(fd);
	return EXIT_SUCCESS;
}
