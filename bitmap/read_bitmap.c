
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define HEADER_SIZE 54
int main(int argc, char* argv[]) {
	//int n = 70;
	int width = 2;
	int height = 2;
	int padding = 2;
	int nChannels = 3;
	
	int _BytesPerRow = width*nChannels  + padding;
	char *data;
	data = (char*)malloc(height*_BytesPerRow*sizeof(char));
	printf("data size %ld\n",sizeof(data));
	
	char filename[] = "BGR24_2x2.bmp";
	FILE *fp = fopen(filename, "rb"); /* b - binary mode */
	if(!fp){
		perror("file not founded");
		return -1;
	}

	char readbuf[100];
	int nread = fread(readbuf, sizeof(char), sizeof(readbuf), fp);
	printf("nread %d\n",nread);
	
	
	
	/* Print image information */
	/* Print header */
	unsigned short int word[1]; /* 2 bytes */
	unsigned int dword[1]; 		/* 4 bytes */
	unsigned char FileType[2];
	unsigned int	Width,	Height;
	/* in bytes */
	unsigned int
			FileHeaderSize,
			FileSize,
			InfoHeaderSize, // header_bytes
			BytesPerRow,
			
			OffsetBits,
			
			nColorPlanes,
			BitsPerPixel,
			CompressType,
			BytesSize, // full size of the file including the header, the pixel data, and all padding
			HorizontalResolution,
			VerticalResolution,
			NumOfUsedColors,
			ImportantColors;
	/* bmp file header */
	fread(FileType,1,2,fp); 								/* file type signature = BM */
	fread(dword,1,4,fp);		FileSize=dword[0];          /* FileSize */
	fread(word,1,2,fp); 									/* reserved1 */
	fread(word,1,2,fp); 									/* reserved2 */
	fread(dword,1,4,fp);		OffsetBits=dword[0];		/* OffsetBits */
	fread(dword,1,4,fp);		InfoHeaderSize=dword[0];
	fread(dword,1,4,fp);		Width=dword[0];
	fread(dword,1,4,fp);		Height=dword[0];
	fread(word,1,2,fp); 		nColorPlanes = word[0]; 	/* planes */	//The number of color planes, must be set to 1
	fread(word,1,2,fp);			BitsPerPixel=word[0];		/* Bits of color per pixel */ //The number of bits per pixel. For an RGB image with a single byte for each color channel the value would be 24
	fread(dword,1,4,fp); 		CompressType=dword[0];		/* compression type */	// 0: no compression
	fread(dword,1,4,fp); 		BytesSize=dword[0];			/* Image Data Size, set to 0 when no compression */
	fread(dword,1,4,fp); 		HorizontalResolution=dword[0];		/* This is the horizontal resolution */
	fread(dword,1,4,fp); 		VerticalResolution=dword[0];		/* This is the vertical resolution */
	fread(dword,1,4,fp); 		NumOfUsedColors=dword[0];			/*  number of used colors, default:0 all colors*/
	fread(dword,1,4,fp); 		ImportantColors=dword[0];			/* The important colors, leave at 0 to default to all colors */



	
	printf("%s \n",readbuf);
	printf("header info:\n");
	printf("file type signature %s\n",FileType);
	
	printf("BytesPerRow= %d\n", BytesPerRow);
	printf("BytesSize= %d\n", BytesSize);
	printf("FileSize= %d\n", FileSize);
	printf("OffsetBits= %d\n", OffsetBits);
	
	int i;
	for(i = 0; i < HEADER_SIZE; i++)
		printf("%x\n",readbuf[i]);
	
	printf("data info:\n");
	int count = 0;
	
	for(; i < nread; i++){
		
		if(count % (_BytesPerRow) == 0)
		{
			printf("\n");
			//count += padding;	// skip padding
		}
			
		
		printf("%.2x  ",readbuf[i]);
		count++;
	}
		
	printf("\n");
	fclose(fp);
	
	
	return 0;
}