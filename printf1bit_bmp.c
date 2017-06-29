/*
 ============================================================================
 Name        : printf1bit_bmp.c
 Author      :
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void fwriteData(FILE* fp,void* data, int width, int height, int nChannels, int padding){
	int n;
	unsigned char *bytes = data;
	int BytesPerRow = width*nChannels  + padding;	//	chia het cho 4
	int BytesSize = BytesPerRow * height;
	if(!fp){
		printf("file not founded");
	}

	for(n = 0; n<BytesSize; n+=BytesPerRow){
		fwrite(bytes+n,1,BytesPerRow,fp);
	}
}

// unsigned char : 1byte, short int 2 bytes, int 4 bytes, long int 8 bytes;	for Intel
int createBMPfile(char* filename, void* data, int width, int height, int nChannels, int padding){
	 //static unsigned char color[1], /* 1 byte = 8 bits */
	        //                     byte[1];

	unsigned short int word[1]; /* 2 bytes */
	unsigned int dword[1]; 		/* 4 bytes */

	unsigned int	Width= width,	Height=height;
	unsigned char *bytes = data;

	unsigned int   bpp=24,	//1,
				   /* in bytes */
				   FileHeaderSize=14,
				   InfoHeaderSize=40, // header_bytes
				   BytesPerRow,
				   FileSize,
				   OffsetBits,
				   BytesSize; // full size of the file including the header, the pixel data, and all padding

	//unsigned int	padding = 2,
	//				nChannels = 3;	// 24bits = 3channels x 8bits

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
	printf("file saved bmp2x2.bmp\n");
	return 0;
}

#define RED {0x00, 0x00, 0xFF}
#define GREEN {0x00, 0xFF, 0x00}
#define BLUE {0xFF, 0x00, 0x00}
#define PURPLE {0xFF, 0x00, 0xFF}
int main() {
	  //                          Blue                Purple
	  //                       B     G     R      B    G     R     Padding
	  //                      |--------------|  |--------------|  |--------|
	 unsigned char data[] = {0xFF, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00,

	  /*                          Red                  Green*/
	  /*                       B     G     R      B    G     R     Padding*/
	  /*                      |--------------|  |--------------|  |--------|*/
	                         0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x00
	  };

	 unsigned char pixel_4x2[] = {
			 0xFF, 0x00, 0x00,	 0xFF, 0x00, 0xFF,		0x00, 0x00, 0xFF, 		0x00, 0xFF, 0x00,// 0x00, 0x00,
			 0x00, 0x00, 0xFF,	 0x00, 0xFF, 0x00,		0xFF, 0x00, 0x00,		0xFF, 0x00, 0xFF,// 0x00, 0x00
	 };

	 unsigned char pixel_3x2[] = {
	 			 RED,RED, RED,	RED,//0x00, 0x00,0x00,
	 			GREEN, GREEN, GREEN,	GREEN//0x00, 0x00,0x00
	 	 };

	 int nChannels = 3;
	 int width = 4, height = 2, padding = 0;
	 createBMPfile("bmp2x2.bmp", pixel_4x2, width, height, nChannels, padding);

        //getchar();
      return 0;
 }
