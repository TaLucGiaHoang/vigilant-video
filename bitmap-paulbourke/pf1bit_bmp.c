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

*/


#include <stdio.h>



 int main() {


        static unsigned char color[1], /* 1 byte = 8 bits */
                             byte[1];
        unsigned short word[1]; /* 2 bytes */
        unsigned long  dword[1], /* 4 bytes */
                       bpp=1,
                       /* in bytes */
                       FileHeaderSize=14,
                       InfoHeaderSize=40, // header_bytes
                       PaletteSize=pow(2,bpp)*4, // = 8 = number of bytes in palette
                       BytesPerRow,
                       FileSize,
                       OffBits,
                       BytesSize, // bytes in image portion
                       iByte, // number of byte
                       /* in pixels */
                       Width= 1000,
                       Height=1000,
                       /* in ... */
                       ix, iy, ix_,
                       bit=0, /* bit value */
                       bitNumber; /* bits are numberd from 0 to 7 */
        //-----------------------
        BytesPerRow=(((Width * bpp)+31)/32)*4;
        printf("BytesPerRow= %d\n", BytesPerRow);
        BytesSize=BytesPerRow*Height;
        printf("BytesSize= %d\n", BytesSize);
        FileSize=FileHeaderSize+InfoHeaderSize+PaletteSize+BytesSize;
        printf("FileSize= %d\n", FileSize);
        OffBits=FileHeaderSize+InfoHeaderSize+PaletteSize;
        printf("OffBits= %d\n", OffBits);
        unsigned char bytes[BytesSize];
        //--------------------------
        FILE *fp = fopen("b12.bmp", "wb"); /* b - binary mode */
        /* bmp file header */
        word[0]=19778;                                         fwrite(word,1,2,fp); /* file Type signature = BM */
        dword[0]=FileSize;                                     fwrite(dword,1,4,fp); /* FileSize */
        word[0]=0;                                             fwrite(word,1,2,fp); /* reserved1 */
        word[0]=0;                                             fwrite(word,1,2,fp); /* reserved2 */
        dword[0]=OffBits;                                      fwrite(dword,1,4,fp); /* OffBits */
        dword[0]=InfoHeaderSize;                               fwrite(dword,1,4,fp);
        dword[0]=Width;                                        fwrite(dword,1,4,fp);
        dword[0]=Height;                                       fwrite(dword,1,4,fp);
        word[0]=1;                                             fwrite(word,1,2,fp); /* planes */
        word[0]=1;                                             fwrite(word,1,2,fp); /* Bits of color per pixel */
        dword[0]=0;                                            fwrite(dword,1,4,fp); /* compression type */
        dword[0]=0;                                            fwrite(dword,1,4,fp); /* Image Data Size, set to 0 when no compression */
        dword[0]=0;                                            fwrite(dword,1,4,fp); /*  */
        dword[0]=0;                                            fwrite(dword,1,4,fp); /*  */
        dword[0]=2;                                            fwrite(dword,1,4,fp); /*  number of used coloors*/
        dword[0]=0;                                            fwrite(dword,1,4,fp); /*  */

        /*  color table (palette) = 2 colors as a RGBA */
        /* color 0 = white */
        byte[0]=255;                                            fwrite(byte,1,1,fp); /* R */
        byte[0]=255;                                            fwrite(byte,1,1,fp); /* G */
        byte[0]=255;                                            fwrite(byte,1,1,fp); /* B */
        byte[0]=255;                                          fwrite(byte,1,1,fp); /*  A*/
        /* color 1 = black */
        byte[0]=0;                                              fwrite(byte,1,1,fp); /* R */
        byte[0]=0;                                              fwrite(byte,1,1,fp); /*  G*/
        byte[0]=0;                                              fwrite(byte,1,1,fp); /* B */
        byte[0]=255;                                              fwrite(byte,1,1,fp); /* A */
        /* pixel color  data  */
        /* clear all bytes */
        for(iByte=0; iByte< BytesSize; iByte++)  bytes[iByte]=0;

        /* compute color of pixela and save it to bytes array */
        for(iy=0 ; iy<Height; iy++){
          ix=0;

          for(iByte=0; iByte< BytesPerRow; iByte++){
            bitNumber=7;


              while (bitNumber<8)
                  {
                   if (sqrt(ix*ix+iy*iy)>500.0) bytes[iByte]+=(int)pow(2,bitNumber);
                    bitNumber-=1;
                    ix+=1;
                    }




        }}


        fwrite(bytes,1,BytesSize,fp);
        fclose(fp);
        printf("file saved\n");
        getchar();
        return 0;
 }


