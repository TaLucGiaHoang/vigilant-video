
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define HEADER_SIZE 54

int freadData(FILE* fp,void* data, int width, int height, int nChannels, int padding){
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

/*
 * data: output 
 */
int readBMPFile(char* filename, char* data, int width, int height, int nChannels, int padding)
{
	/* Print image information */
	/* Print header */
	unsigned short int word[1]; /* 2 bytes */
	unsigned int dword[1]; 		/* 4 bytes */
	unsigned char FileType[2];
	unsigned int	Width,	Height;
	int total_bytes = 0;
	/* in bytes */
	unsigned int
			//FileHeaderSize,
			InfoHeaderSize,
			FileSize,// full size of the file including the header, the pixel data, and all padding
			//BytesPerRow,		
			OffsetBits,		
			nColorPlanes,
			BitsPerPixel,
			CompressType,
			BytesSize, 
			HorizontalResolution,
			VerticalResolution,
			NumOfUsedColors,
			ImportantColors;

	/* Open file */
	FILE *fp = fopen(filename, "rb"); /* b - binary mode */
	if(!fp){
		perror("file not founded");
		return -1;
	}

	/* bmp file header */
	fread(FileType,1,2,fp); 								/* file type signature = BM */
	fread(dword,1,4,fp);		FileSize=dword[0];          /* FileSize */
	fread(word,1,2,fp); 									/* reserved1 */
	fread(word,1,2,fp); 									/* reserved2 */
	fread(dword,1,4,fp);		OffsetBits=dword[0];		/* OffsetBits */
	fread(dword,1,4,fp);		InfoHeaderSize=dword[0];
	fread(dword,1,4,fp);		Width=dword[0];				/* Width */
	fread(dword,1,4,fp);		Height=dword[0];			/* Height */
	fread(word,1,2,fp); 		nColorPlanes = word[0]; 	/* planes */	//The number of color planes, must be set to 1
	fread(word,1,2,fp);			BitsPerPixel=word[0];		/* Bits of color per pixel */ //The number of bits per pixel. For an RGB image with a single byte for each color channel the value would be 24
	fread(dword,1,4,fp); 		CompressType=dword[0];		/* compression type */	// 0: no compression
	fread(dword,1,4,fp); 		BytesSize=dword[0];			/* Image Data Size, set to 0 when no compression */
	fread(dword,1,4,fp); 		HorizontalResolution=dword[0];		/* This is the horizontal resolution */
	fread(dword,1,4,fp); 		VerticalResolution=dword[0];		/* This is the vertical resolution */
	fread(dword,1,4,fp); 		NumOfUsedColors=dword[0];			/*  number of used colors, default:0 all colors*/
	fread(dword,1,4,fp); 		ImportantColors=dword[0];			/* The important colors, leave at 0 to default to all colors */




	printf("header info:\n");
	printf("FileType %c%c\n",FileType[0],FileType[1]);
	printf("FileSize = %d\n", FileSize);
	printf("OffsetBits = %d\n", OffsetBits);
	printf("InfoHeaderSize = %d\n", InfoHeaderSize);
	printf("Width = %d\n", Width);
	printf("Height = %d\n", Height);
	printf("nColorPlanes = %d\n", nColorPlanes);
	printf("BitsPerPixel = %d\n", BitsPerPixel);
	// printf("CompressType = %d\n", CompressType);
	printf("BytesSize = %d\n", BytesSize);
	// printf("HorizontalResolution = %d\n", HorizontalResolution);
	// printf("VerticalResolution = %d\n", VerticalResolution);
	// printf("NumOfUsedColors = %d\n", NumOfUsedColors);
	// printf("ImportantColors = %d\n", ImportantColors);	
	
	/* data info */
	
	//int width = Width;
	//int height = Height;
	//int padding = 2;
	//int nChannels = 3;		// 24bit RGB
	//char *data;
	int BytesPerRow = width*nChannels  + padding;
	int dataSize = height*BytesPerRow*sizeof(char);	// equal BytesSize
	printf("dataSize = %d\n", dataSize);

	data = (char*)malloc(BytesSize);//data = (char*)malloc(dataSize);

	//fread(data,dataSize,sizeof(char),fp);	// READ ALL DATA 

	printf("read data per line\n" );
	int n;
	for(n = BytesPerRow; n <= BytesSize; n+=BytesPerRow){
		total_bytes += fread(data+BytesSize-n,1,BytesPerRow,fp);	// khong doc padding
	}
	int i;
	
	printf("data info:\n");
	int CountInLine = 0;
	//freadData(fp,)
 	for(; i < dataSize; i++, CountInLine++){
		
		if(i % (width*nChannels) == 0)
		{
			printf("\n");
			//i += 2;	// skip padding
		}
		//else	
			printf("%.2x  ",data[i]);
		///count++;
	} 
		

		

	printf("\n");
	fclose(fp);
	return 0;
}
int main(int argc, char* argv[]) {
	//int n = 70;

	

	
	char filename[] = "BGR24_2x2.bmp";


	int width = 2;
	int height = 2;
	int padding = 2;
	int nChannels = 3;		// 24bit RGB
	char* data = NULL;
readBMPFile(filename, data, width, height, nChannels, padding);
	
	
	

	
	
	return 0;
}