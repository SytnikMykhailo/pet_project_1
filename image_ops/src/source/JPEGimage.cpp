#include "../headers/JPEGimage.hpp"

std::unique_ptr<Image> JPEGImage::load(const std::string &path_to_image){
    std::cout << path_to_image << std::endl;
    throw std::runtime_error("JPEGImage::load is not implemented yet.");
}

void JPEGImage::setPixelColor(Color new_color, int pos_x, int pos_y) {
    if (pos_x >= 0 && pos_x < width && pos_y >= 0 && pos_y < height) {
        pixels[pos_y * width + pos_x] = new_color;
    } else {
        std::cerr << "setPixelColor: position out of bounds\n";
    }
}

Color JPEGImage::getPixelColor(int pos_x, int pos_y) const {
    if (pos_x >= 0 && pos_x < width && pos_y >= 0 && pos_y < height) {
        return pixels[pos_y * width + pos_x];
    } else {
        std::cerr << "getPixelColor: position out of bounds\n";
        return Color();
    }
}

void JPEGImage::save(const std::string &path_to_folder) const {
    std::cout << path_to_folder << std::endl;
    throw std::runtime_error("JPEGImage::load is not implemented yet.");
}

JPEGImage::~JPEGImage(){
    
}

JPEGImage::JPEGImage(int _height, int _width):
    height{_height}, width{_width}
{
    pixels.resize(width * height);
}