#include "../headers/JPEGimage.hpp"

std::unique_ptr<Image> JPEGImage::load(const std::string &path_to_image){
    std::cout << path_to_image << std::endl;
    return nullptr;
}

void JPEGImage::setPixelColor(Color new_color, int pos_x, int pos_y) {
    std::cout << pos_x << pos_y << new_color.getRed() <<  std::endl;
}

Color JPEGImage::getPixelColor(int pos_x, int pos_y) const {
    std::cout << pos_x << pos_y <<  std::endl;
    return Color();
}

void JPEGImage::save(const std::string &path_to_folder) const {
    std::cout << path_to_folder << std::endl;
}

JPEGImage::~JPEGImage(){
    
}

JPEGImage::JPEGImage(int _height, int _width):
    height{_height}, width{_width}
{
    pixels.resize(width * height);
}