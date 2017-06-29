/*
 * Copyright (c) 2012 Tom Alexander
 * 
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 * 
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 
 *    1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 
 *    2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 
 *    3. This notice may not be removed or altered from any source
 *    distribution.
 */
#include <cinttypes>
#include <fstream>

size_t bitmap_encode_rgb(const uint8_t* rgb, int width, int height, uint8_t** output);

int main(int argc, char** argv)
{
    //                      Red              Green
    //                |---------------| |--------------|
    uint8_t data[] = {0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00,
                      0x00, 0x00, 0xFF, 0xFF, 0x00, 0xFF};
    //                |--------------|  |--------------|
    //                     Blue              Purple
    uint8_t* output;
    size_t output_size = bitmap_encode_rgb(data, 2, 2, &output);
    
    std::ofstream file_output;
    file_output.open("output.bmp");
    file_output.write((const char*)output, output_size);
    file_output.close();
    
    delete [] output;
    return 0;
}
