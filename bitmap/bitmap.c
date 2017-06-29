/*
 ============================================================================
 Name        : bitmap.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct myBitmap {
	//14byte bitmap header
	unsigned char byte_0 ;//= 0x42;	//'B'
	unsigned char byte_1 ;//= 0x4D;	//'M'
	size_t file_size_offset ;//= 0 ;//data.size() //	This will be the full size of the file including the header, the pixel data, and all padding
	unsigned int byte_6_9 ;//= 0x00000000;
	size_t pixel_info_offset_offset ;//= 54;//data.size();
	//40byte bitmap information
	unsigned char BitmapInfoHeader ;//= 40;	//byte 14-17
	//Color Palette 4x bytes
	unsigned int width;	//byte 18-21
	unsigned int height;	//byte 22-25
	//byte 26-27: the number of color plantes
	unsigned char byte_26 ;//= 1;
	unsigned char byte_27 ;//= 0;
	//byte 28-29: the number of bits per pixel
	unsigned char byte_28 ;//= 24;
	unsigned char byte_29 ;//= 0;
	//byte 30-33 disable compression
	unsigned int unCompress ;//= 0;
	size_t raw_pixel_array_size_offset ;//= 0;//size of raw data in pixel array, fill later
	//byte 38-41
	unsigned int horizontal_resolution ;//= 2835;
	//byte 42-45
	unsigned int vertical_resolution ;//= 2835;
	//byte 46-49
	unsigned int num_of_color ;//= 0;	//default to all colors
	//byte 50-53
	unsigned int important_color ;//= 0;	//default to all colors

	//Bitmap Data
	void* data;
} Bitmap;


 void createBitmap(void* data,size_t data_size, int width ,int height,Bitmap output){
		//14byte bitmap header
		output.byte_0 = 0x42;	//'B'
		output.byte_1 = 0x4D;	//'M'
		size_t file_size_offset = data_size;
		//byte 2-5 file size
		output.file_size_offset = 0xFFFFFFFF ;//	This will be the full size of the file including the header, the pixel data, and all padding
		//byte 6-9
		output.byte_6_9 = 0x00000000;
		size_t pixel_info_offset_offset = data_size;
		//byte 10-13 pixel offset
		output.pixel_info_offset_offset = 0;	//fill later
		//40byte bitmap information
		output.BitmapInfoHeader = 40;	//byte 14-17
		//Color Palette 4x bytes
		output.width = width;	//byte 18-21
		output.height = height;	//byte 22-25
		//byte 26-27: the number of color plantes
		output.byte_26 = 1;
		output.byte_27 = 0;
		//byte 28-29: the number of bits per pixel
		output.byte_28 = 24;
		output.byte_29 = 0;
		//byte 30-33 disable compression
		output.unCompress = 0;
		size_t raw_pixel_array_size_offset = data_size;
		output.raw_pixel_array_size_offset = 0;//size of raw data in pixel array, fill later
		//byte 38-41
		output.horizontal_resolution = 2835;
		//byte 42-45
		output.vertical_resolution = 2835;
		//byte 46-49
		output.num_of_color = 0;	//default to all colors
		//byte 50-53
		output.important_color = 0;	//default to all colors

		uint32_t _data_size = data_size;


		//Bitmap Data
		output.data = data;
}
int main(int argc, char** argv)
{
	  //                          Blue                Purple
	  //                       B     G     R      B    G     R     Padding
	  //                      |--------------|  |--------------|  |--------|
	 // unsigned char data[] = {0xFF, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00,

	  /*                          Red                  Green*/
	  /*                       B     G     R      B    G     R     Padding*/
	  /*                      |--------------|  |--------------|  |--------|*/
	 //                         0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00
	//  };
    //unsigned char* output;
//    size_t output_size ;//= bitmap_encode_rgb(data, 2, 2, &output);
    //Bitmap file_bmp;

    //void* data = header;
//    FILE* file_output;
//    file_output = fopen("output.bmp","w+");

    /*
    ptr − This is the pointer to the array of elements to be written.

    size − This is the size in bytes of each element to be written.

    nmemb − This is the number of elements, each one with a size of size bytes.

    stream − This is the pointer to a FILE object that specifies an output stream.
    */
//    fwrite(data,1,sizeof(output_size),file_output);
    //file_output.write((const char*)output, output_size,);
//    fclose(file_output);

    return 0;
}

