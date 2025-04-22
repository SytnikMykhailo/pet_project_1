#include "../headers/imageloader.hpp"


std::unique_ptr<Image> ImageLoader::load(const std::string &path){
    if (path.ends_with(".bmp")) {
        return BMPImage::load(path);
    } else if (path.ends_with(".png")) {
        return PNGImage::load(path);
    } else if (path.ends_with(".jpeg") || path.ends_with(".jpg")) {
        return JPEGImage::load(path);
    } else {
        throw std::runtime_error("Unsupported image format");
    }
}

void ImageLoader::save(const Image &image, std::string &path){
    image.save(path);
}