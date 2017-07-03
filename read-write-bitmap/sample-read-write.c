
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define HEADER_SIZE 54

typedef struct _BITMAP{
	//unsigned int FileHeaderSize;
	unsigned char   FileType[2];
	unsigned int	InfoHeaderSize;
	unsigned int	FileSize;// full size of the file including the header, the pixel data, and all padding
	unsigned int	BytesPerRow;
	unsigned int	OffsetBits;
	unsigned int	Width;
	unsigned int	Height;
	unsigned int	nColorPlanes;
	unsigned int	BitsPerPixel;
	unsigned int	CompressType;
	unsigned int	DataSize;
	unsigned int	HorizontalResolution;
	unsigned int	VerticalResolution;
	unsigned int	NumOfUsedColors;
	unsigned int	ImportantColors;
	unsigned char   *Data;
} BITMAP;


int processImage(const unsigned char  *dataIn, unsigned char *dataOut, int dataSize);
void printBMPInfo(BITMAP* img);
int readBMPFile(char* filename, BITMAP *img, int nChannels, int padding);
int writeBMPfile(char* filename, void* data, int width, int height, int nChannels, int padding);



int processImage(const unsigned char  *dataIn, unsigned char *dataOut, int dataSize)
{
	int i;
	if(dataOut == NULL)
	{
		dataOut = malloc(dataSize);
	}
	
	*dataOut = 250;
	printf("dataout %p  %d %p\n",dataOut, *dataOut, &dataOut);
	
	for(i = 0; i < dataSize; i++ )
	{
		dataOut[i] = 255 - dataIn[i];	// increase brightness			
	}
	
	
	return 0;
}

/* Print image information */
void printBMPInfo(BITMAP* img)
{
	int i;	
	/* Print header */
	printf("header info:\n");
	printf("FileType %c%c\n",img->FileType[0],img->FileType[1]);
	printf("FileSize = %d\n", img->FileSize);
	printf("OffsetBits = %d\n", img->OffsetBits);
	printf("InfoHeaderSize = %d\n", img->InfoHeaderSize);
	printf("Width = %d\n", img->Width);
	printf("Height = %d\n", img->Height);
	//printf("nColorPlanes = %d\n", img->nColorPlanes);
	printf("BitsPerPixel = %d\n", img->BitsPerPixel);
	// printf("CompressType = %d\n", img->CompressType);
	printf("DataSize = %d\n", img->DataSize);
	// printf("HorizontalResolution = %d\n", img->HorizontalResolution);
	// printf("VerticalResolution = %d\n", img->VerticalResolution);
	// printf("NumOfUsedColors = %d\n", img->NumOfUsedColors);
	// printf("ImportantColors = %d\n", img->ImportantColors);	
		
	/* Print data */
	printf("data info:\n");
 	for(i = 0; i < 50; i++){
		//if(i % img->BytesPerRow == 0)
		//	printf("\n");
		printf("%.2x  ",img->Data[i]);
	}
	printf("\n");
}


int readBMPFile(char* filename, BITMAP *img, int nChannels, int padding)
{
	unsigned short int word[1]; /* 2 bytes */
	unsigned int dword[1]; 		/* 4 bytes */
	int total_bytes = 0;

	/* Open file */
	FILE *fp = fopen(filename, "rb"); /* b - binary mode */
	if(!fp){
		perror("file not founded");
		return -1;
	}

	/* bmp file header */
	fread(img->FileType,1,2,fp); 								/* file type signature = BM */
	fread(dword,1,4,fp);		img->FileSize=dword[0];          /* FileSize */
	fread(word,1,2,fp); 									/* reserved1 */
	fread(word,1,2,fp); 									/* reserved2 */
	fread(dword,1,4,fp);		img->OffsetBits=dword[0];		/* OffsetBits */
	fread(dword,1,4,fp);		img->InfoHeaderSize=dword[0];
	fread(dword,1,4,fp);		img->Width=dword[0];				/* Width */
	fread(dword,1,4,fp);		img->Height=dword[0];			/* Height */
	fread(word,1,2,fp); 		img->nColorPlanes = word[0]; 	/* planes */	//The number of color planes, must be set to 1
	fread(word,1,2,fp);			img->BitsPerPixel=word[0];		/* Bits of color per pixel */ //The number of bits per pixel. For an RGB image with a single byte for each color channel the value would be 24
	fread(dword,1,4,fp); 		img->CompressType=dword[0];		/* compression type */	// 0: no compression
	fread(dword,1,4,fp); 		img->DataSize=dword[0];			/* Image Data Size, set to 0 when no compression */
	fread(dword,1,4,fp); 		img->HorizontalResolution=dword[0];		/* This is the horizontal resolution */
	fread(dword,1,4,fp); 		img->VerticalResolution=dword[0];		/* This is the vertical resolution */
	fread(dword,1,4,fp); 		img->NumOfUsedColors=dword[0];			/*  number of used colors, default:0 all colors*/
	fread(dword,1,4,fp); 		img->ImportantColors=dword[0];			/* The important colors, leave at 0 to default to all colors */

	/* data info */
	img->BytesPerRow = img->Width*nChannels  + padding;
	img->Data = (unsigned char*)malloc(img->DataSize);//data = (char*)malloc(dataSize);
	int n;
	for(n = img->BytesPerRow; n <= img->DataSize; n+=img->BytesPerRow){
		total_bytes += fread(img->Data+img->DataSize-n,1,img->BytesPerRow,fp);
	}
	fclose(fp);
	return 0;
}


/*Notes: unsigned char : 1byte, short int 2 bytes, int 4 bytes, long int 8 bytes;	for Intel */
int writeBMPfile(char* filename, void* data, int width, int height, int nChannels, int padding)
{
	unsigned short int word[1]; /* 2 bytes */
	unsigned int dword[1]; 		/* 4 bytes */

	unsigned int	Width= width,	Height=height;
	/* in bytes */
	unsigned int   FileHeaderSize=14,
			InfoHeaderSize=40, // header_bytes
			BytesPerRow,
			FileSize,
			OffsetBits,
			DataSize; // full size of the file including the header, the pixel data, and all padding

	//PRINT
	BytesPerRow = Width*nChannels  + padding;	//(((Width * bpp)+31)/32)*4;	chia het cho 4
	DataSize = BytesPerRow*Height;
	FileSize = FileHeaderSize+InfoHeaderSize+DataSize;	//+PaletteSize;
	OffsetBits = FileHeaderSize+InfoHeaderSize;//+PaletteSize;

	//	printf("BytesPerRow= %d\n", BytesPerRow);
	//	printf("DataSize= %d\n", DataSize);
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
	dword[0]=DataSize;                                    fwrite(dword,1,4,fp); /* Image Data Size, set to 0 when no compression */
	dword[0]=2835;                                         fwrite(dword,1,4,fp); /* This is the horizontal resolution */
	dword[0]=2835;                                         fwrite(dword,1,4,fp); /* This is the vertical resolution */
	dword[0]=0;                                            fwrite(dword,1,4,fp); /*  number of used colors, default:0 all colors*/
	dword[0]=0;                                            fwrite(dword,1,4,fp); /* The important colors, leave at 0 to default to all colors */

	//pixel data
	int data_size;
	int n;
	for(n = BytesPerRow; n <= DataSize; n+=BytesPerRow){
		data_size += fwrite(data+DataSize-n,1,BytesPerRow,fp);
	}
	
	fclose(fp);
	printf("file saved %s\n",filename);
	return data_size + 54;	// header 54 bytes
}


int main(int argc, char* argv[]) {
	//int n = 70;


	char lena[30] = "Lenna-512x512-24bit.bmp";
	char filename[] = "BGR24_2x2.bmp";

	//int width = 2;
	//int height = 2;
	//int padding = 2;
	//int nChannels = 3;		// 24bit RGB
	//char* data = NULL;
	//readBMPFile(filename, &data, nChannels, padding);


	
	BITMAP img;
	
	printf("read image file\n");
	readBMPFile(lena, &img , 3,0);
	printBMPInfo(&img);

	
	printf("processing\n");
	processImage(img.Data, img.Data, img.DataSize);

	printf("save image file\n");
	writeBMPfile("Output-Lenna-512x512-24bit.bmp", img.Data, img.Width, img.Height, 3, 0);





	return 0;
}



// int readBMPFile(char* filename, char** outData,int nChannels, int padding)
// {
	// /* Print image information */
	// /* Print header */
	// char* data = outData[0];
	// unsigned short int word[1]; /* 2 bytes */
	// unsigned int dword[1]; 		/* 4 bytes */
	// unsigned char FileType[2];
	// unsigned int	Width,	Height;
	// int total_bytes = 0;
	// /* in bytes */
	// unsigned int
			////FileHeaderSize,
			// InfoHeaderSize,
			// FileSize,// full size of the file including the header, the pixel data, and all padding
			////BytesPerRow,
			// OffsetBits,
			// nColorPlanes,
			// BitsPerPixel,
			// CompressType,
			// DataSize,
			// HorizontalResolution,
			// VerticalResolution,
			// NumOfUsedColors,
			// ImportantColors;

	// /* Open file */
	// FILE *fp = fopen(filename, "rb"); /* b - binary mode */
	// if(!fp){
		// perror("file not founded");
		// return -1;
	// }

	// /* bmp file header */
	// fread(FileType,1,2,fp); 								/* file type signature = BM */
	// fread(dword,1,4,fp);		FileSize=dword[0];          /* FileSize */
	// fread(word,1,2,fp); 									/* reserved1 */
	// fread(word,1,2,fp); 									/* reserved2 */
	// fread(dword,1,4,fp);		OffsetBits=dword[0];		/* OffsetBits */
	// fread(dword,1,4,fp);		InfoHeaderSize=dword[0];
	// fread(dword,1,4,fp);		Width=dword[0];				/* Width */
	// fread(dword,1,4,fp);		Height=dword[0];			/* Height */
	// fread(word,1,2,fp); 		nColorPlanes = word[0]; 	/* planes */	//The number of color planes, must be set to 1
	// fread(word,1,2,fp);			BitsPerPixel=word[0];		/* Bits of color per pixel */ //The number of bits per pixel. For an RGB image with a single byte for each color channel the value would be 24
	// fread(dword,1,4,fp); 		CompressType=dword[0];		/* compression type */	// 0: no compression
	// fread(dword,1,4,fp); 		DataSize=dword[0];			/* Image Data Size, set to 0 when no compression */
	// fread(dword,1,4,fp); 		HorizontalResolution=dword[0];		/* This is the horizontal resolution */
	// fread(dword,1,4,fp); 		VerticalResolution=dword[0];		/* This is the vertical resolution */
	// fread(dword,1,4,fp); 		NumOfUsedColors=dword[0];			/*  number of used colors, default:0 all colors*/
	// fread(dword,1,4,fp); 		ImportantColors=dword[0];			/* The important colors, leave at 0 to default to all colors */




	// printf("header info:\n");
	// printf("FileType %c%c\n",FileType[0],FileType[1]);
	// printf("FileSize = %d\n", FileSize);
	// printf("OffsetBits = %d\n", OffsetBits);
	// printf("InfoHeaderSize = %d\n", InfoHeaderSize);
	// printf("Width = %d\n", Width);
	// printf("Height = %d\n", Height);
	////printf("nColorPlanes = %d\n", nColorPlanes);
	// printf("BitsPerPixel = %d\n", BitsPerPixel);
	////printf("CompressType = %d\n", CompressType);
	// printf("DataSize = %d\n", DataSize);
	////printf("HorizontalResolution = %d\n", HorizontalResolution);
	////printf("VerticalResolution = %d\n", VerticalResolution);
	////printf("NumOfUsedColors = %d\n", NumOfUsedColors);
	////printf("ImportantColors = %d\n", ImportantColors);

	// /* data info */

	// int BytesPerRow = Width*nChannels  + padding;

	// data = (char*)malloc(DataSize);//data = (char*)malloc(dataSize);

	////fread(data,DataSize,sizeof(char),fp);	// READ ALL DATA


	// int n;
	// for(n = BytesPerRow; n <= DataSize; n+=BytesPerRow){
		// total_bytes += fread(data+DataSize-n,1,BytesPerRow,fp);
	// }
	// int i;

	// printf("data info:\n");
 	// for(i = 0; i < DataSize; i++){
		// if(i % BytesPerRow == 0)
			// printf("\n");
		// printf("%.2x  ",data[i]);
	// }




	// printf("\n");
	// fclose(fp);
	///////////////////////////////////////////////////////////////////////////////////
	////InvertImage(&data, &data, Width, Height, nChannels, padding);
		// for(i = 0; i < DataSize/2; i++ )
	// {
		// data[i] = 250;	// increase brightness
			
	// }
	// writeBMPfile("outputImage.bmp",data, Width, Height, nChannels, padding);
	
	// return 0;
// }