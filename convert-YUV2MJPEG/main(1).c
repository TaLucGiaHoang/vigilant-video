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

int clamp (double x){
    int r = x;
    if(r<0) return 0;
    else if(r>255)  return 255;
    else return r;
}
    int Y1, Cb, Cr;
    int ER, EG, EB;
void YCbCr2RGB(void){
    double r,g,b;
    double y1,pb,pr;

    y1 = (255 / 219.0) * (Y1 - 16);
    pb = (255 / 224.0) * (Cb - 128);
    pr = (255 / 224.0) * (Cr - 128);

    r = 1.0 * y1 + 0 * pb + 1.402 * pr;
    g = 1.0 * y1 - 0.344 * pb - 0.714 * pr;
    b = 1.0 * y1 + 1.772 * pb + 0 * pr;

    ER = clamp (r * 255); /* [ok? one should prob. limit y1,pb,pr] */
    EG = clamp (g * 255);
    EB = clamp (b * 255);
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
void yuyv2rgb(unsigned int* yuyv_2_pixels, BITMAP4* rgb1, BITMAP4* rgb2){
    int y1,y2,u,v;
    char r,g,b;
    /*//https://social.msdn.microsoft.com/Forums/windowsdesktop/en-US/1071301e-74a2-4de4-be72-81c34604cde9/program-to-translate-yuyv-to-rgbrgb?forum=windowsdirectshowdevelopment
    v  = ((*yuyv_pixel & 0x000000ff));
    y1  = ((*yuyv_pixel & 0x0000ff00)>>8);
    u  = ((*yuyv_pixel & 0x00ff0000)>>16);
    y2 = ((*yuyv_pixel & 0xff000000)>>24);
    */
    /*//http://paulbourke.net/dataformats/yuv/
    u  = ((*yuyv_pixel & 0x000000ff));
    y1  = ((*yuyv_pixel & 0x0000ff00)>>8);
    v  = ((*yuyv_pixel & 0x00ff0000)>>16);
    y2 = ((*yuyv_pixel & 0xff000000)>>24);
    */
    //http://v4l.videotechnology.com/dwg/v4l2.html#V4L2-PIX-FMT-YUYV
    y1  = ((*yuyv_pixel & 0x000000ff));
    u  = ((*yuyv_pixel & 0x0000ff00)>>8);
    y2  = ((*yuyv_pixel & 0x00ff0000)>>16);
    v = ((*yuyv_pixel & 0xff000000)>>24);
    //yuv to rgb
    *rgb1 = YUV_to_Bitmap(y1,u,v);
    *rgb2 = YUV_to_Bitmap(y2,u,v);
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
	///format.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
	///format.fmt.pix.pixelformat = V4L2_PIX_FMT_JPEG;
	format.fmt.pix.width = 320;//640;//800;
	format.fmt.pix.height = 240;//480;//600;



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

	/* Your loops end here. */
	//}	//end of for loop
	//}	//end of while loop

	/*Save frame as an image*/
	int imagefile;
	if((imagefile  = open("myimage_mjpeg.bmp", O_WRONLY | O_CREAT, 0660)) < 0){
	//if((imagefile = open("myimage.bmp", O_WRONLY | O_CREAT, 0660)) < 0){
	    perror("open");
	    return 1;//exit(1);
	}
	else{
            printf("save file myimage.bmp\n");
            ///write(imagefile, buffer_start, bufferinfo.length);
            ///close(imagefile);
	}

    FILE *textfile;
    textfile = fopen("text.txt","w+");
    int width = format.fmt.pix.width;
    int height = format.fmt.pix.height;
    int count = 0;
    int h = 0, w =0;
    unsigned int *buffer_int = buffer_start;    //4byte
    //unsigned char *buffer_char = buffer_start;    //4byte
    unsigned int _4_byte;
    void *new_buffer;
    memset(new_buffer, 0, width*height*3);
    for( h = 0; h<(bufferinfo.length/height); h++){
        for( w = 0; w<width; w++){
                //unsigned char byte = buffer_start[w+h];
                _4_byte = buffer_int[w+h];
                BITMAP4 rgb1,rgb2;
                yuyv2rgb(&_4_byte,&rgb1,&rgb2);

                //add 2 new rgb pixel to buffer
                _4_byte = (rgb1 & 0x0000ffff)
                //fprintf(textfile,"%x \t",buffer_int[w+h]);
                count++;
            }
        fprintf(textfile,"\n %d | row = %d\n",count,h);
    }
    fprintf(textfile,"count %d | sizeof_1_buff: %d | sizeof(char): %d | sizeof(int): %d",count,sizeof(buffer_int),sizeof(_4_byte),sizeof(int));
    fclose(textfile);

    printf("buffer_info\n"
    "bufferinfo.bytesused: %d\n"
    "bufferinfo.field: %d\n"
    "bufferinfo.flags: %d\n"
    "bufferinfo.index: %d\n"
    "bufferinfo.length: %d\n"
    "bufferinfo.memory: %d\n"
    "bufferinfo.type: %d\n",
    bufferinfo.bytesused,
    bufferinfo.field,
    bufferinfo.flags,
    bufferinfo.index,
    bufferinfo.length,
    bufferinfo.memory,
    bufferinfo.type);

	write(imagefile, buffer_start, bufferinfo.length);
	close(imagefile);

	// Deactivate streaming
	if(ioctl(fd, VIDIOC_STREAMOFF, &type) < 0){
	    perror("VIDIOC_STREAMOFF");
	    return 1;//exit(1);
	}

    close(fd);
    return EXIT_SUCCESS;
}
