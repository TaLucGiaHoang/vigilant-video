# vigilant-video

V4L2_PIX_FMT_YUYV: convert from YUYV to RGB24?
https://stackoverflow.com/questions/16176415/v4l2-pix-fmt-yuyv-convert-from-yuyv-to-rgb24

BMP
https://vi.wikipedia.org/wiki/BMP

Video camera formats: yuv411, yuv422 - Written by Paul Bourke
http://paulbourke.net/dataformats/yuv/


==========================================================================================
Video camera formats: yuv411, yuv422

Written by Paul Bourke
August 2004


There are a number of video formats used by digital video cameras say, perhaps the most common at the time of writing are called yuv444, yuv422, and yuv411. The following briefly describes the layout of the data (luminance and two chrominance channels) and describes how to convert these to the more familiar RGB, noting of course that it may be more appropriate to do some image processing and analysis in YUV (also known as YCbCr space. The basic idea behind these formats is that the human visual system is less sensitive to high frequency colour information (compared to luminance) so the colour information can be encoded at a lower spatial resolution.

422
To reconstruct pixels in pairs, 4 bytes of data are required. The bytes are arranged as u, y1, v, y2. The total number of bytes in the image stream is 2 x width x height.

void Convert422(unsigned char *yuv,BITMAP4 *rgb1,BITMAP4 *rgb2)
{
   int y1,y2,u,v;

   // Extract yuv components
   u  = yuv[0];
   y1 = yuv[1];
   v  = yuv[2];
   y2 = yuv[3];

   // yuv to rgb
   *rgb1 = YUV_to_Bitmap(y1,u,v);
   *rgb2 = YUV_to_Bitmap(y2,u,v);
}
411
To reconstruct pixels in sets of 4, 6 bytes of data are required. The bytes are arranged as u, y1, y2, v, y3, y4. The total number of bytes in the image stream is 6 x width x height / 4.

void Convert411(unsigned char *yuv,BITMAP4 *rgb1,BITMAP4 *rgb2,BITMAP4 *rgb3,BITMAP4 *rgb4)
{  
   int y1,y2,y3,y4,u,v;
   
   // Extract yuv components
   u  = yuv[0];
   y1 = yuv[1];
   y2 = yuv[2];
   v  = yuv[3];
   y3 = yuv[4];
   y4 = yuv[5];

   // yuv to rgb
   *rgb1 = YUV_to_Bitmap(y1,u,v);
   *rgb2 = YUV_to_Bitmap(y2,u,v);
   *rgb3 = YUV_to_Bitmap(y3,u,v);
   *rgb4 = YUV_to_Bitmap(y4,u,v);
}
Conversion to RGB
The plain C source provided here is written for clarify not efficiency, in general these conversions would be performed by the way of lookup tables.

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

   bm.r = r;
   bm.g = g;
   bm.b = b;
   bm.a = 0;

   return(bm);
}
