/*
  C console program
  A plain black and white bitmap requires only a single bit to document each pixel.
  the bitmap is monochrome, and the palette contains two entries.
  each bit in the bitmap array represents a pixel. if the bit is clear,
  the pixel is displayed with the color of the first entry in the palette;
  if the bit is set, the pixel has the color of the second entry in the table.
  This type of bitmap has a very small file size, and doesn't require a palette.
  based on program bmpsuite.c by Jason Summers
  http://entropymine.com/jason/bmpsuite/
  Adam Majewski
  fraktal.republika.pl
  This app create a 2x2-pixel 24-bit BGR image
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

int fwriteBMPData(FILE* fp,void* data, int width, int height, int nChannels, int padding){
	int n;
	int total_bytes = 0;
	unsigned char *bytes = data;
	int BytesPerRow = width*nChannels  + padding;	//	chia het cho 4
	int BytesSize = BytesPerRow * height;
	if(!fp){
		perror("file not founded");
	}
	else
		for(n = BytesPerRow; n <= BytesSize; n+=BytesPerRow){
			total_bytes += fwrite(bytes+BytesSize-n,1,BytesPerRow,fp);
		}

	return total_bytes;
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
	BytesSize = BytesPerRow*Height;	//kich thuoc data anh tinh theo byte
	FileSize = FileHeaderSize+InfoHeaderSize+BytesSize;	//+PaletteSize;
	OffsetBits = FileHeaderSize+InfoHeaderSize;//+PaletteSize;

	//	printf("BytesPerRow= %d\n", BytesPerRow);
	//	printf("BytesSize= %d\n", BytesSize);
	//	printf("FileSize= %d\n", FileSize);
	//	printf("OffsetBits= %d\n", OffsetBits);


	FILE *fp = fopen(filename, "wb"); /* b - binary mode */
	if(!fp){
		perror("file not founded");
		return -1;
	}
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

	//pixel data
	int data_size;
	data_size = fwriteBMPData(fp,data,Width,Height,nChannels,padding); // bmp image data need to be inverted when writing  

	fclose(fp);
	printf("file saved %s\n",filename);
	return data_size + 54;	// header 54 bytes
}


int main() {

	//                          Blue                Purple
	//                       B     G     R      B    G     R     Padding
	//                      |--------------|  |--------------|  |--------|
	unsigned char data[] = {0xff, 0x02, 0x03, 0x04, 0xff, 0x06, 0x00, 0x00,

	//                          Red                  Green
	//                       B     G     R      B    G     R     Padding
	//                      |--------------|  |--------------|  |--------|
				0x09, 0x10, 0xff, 0x12, 0x13, 0x14, 0x15, 0x16
	};

	int n = createBMPfile("BGR24_2x2.bmp",data,2,2,3,2); 	// image size = 16 data bytes + 54 header bytes = 70 bytes
	printf("total_bytes %d\n",n);

	return 0;
}