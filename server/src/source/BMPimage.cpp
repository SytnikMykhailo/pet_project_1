#include "../headers/BMPimage.hpp"

BMPImage::BMPImage(int _height, int _width):
    height{_height}, width{_width}
{
    pixels.resize(width * height);
}



BMPImage::~BMPImage(){
    
}
