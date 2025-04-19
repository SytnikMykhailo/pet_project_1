#include "../headers/PNGimage.hpp"


std::unique_ptr<Image> PNGImage::load(const std::string &path_to_image){
    std::cout << path_to_image << std::endl;
    return nullptr;
}

void PNGImage::setPixelColor(Color new_color, int pos_x, int pos_y) {
    std::cout << pos_x << pos_y << new_color.getRed() <<  std::endl;
}

Color PNGImage::getPixelColor(int pos_x, int pos_y) const {
    std::cout << pos_x << pos_y <<  std::endl;
    return Color();
}

void PNGImage::save(const std::string &path_to_folder) const {
    std::cout << path_to_folder << std::endl;
}

PNGImage::~PNGImage(){
    
}

PNGImage::PNGImage(int _height, int _width):
    height{_height}, width{_width}
{
    pixels.resize(width * height);
}

uint32_t readBigEndianUint32(std::istream& stream) {
    uint8_t bytes[4];
    stream.read(reinterpret_cast<char*>(bytes), 4);
    return (bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | (bytes[3]);
}