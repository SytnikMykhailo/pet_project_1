#include "BMPimage.hpp"

class ImageLoader{
    static std::unique_ptr<Image> load(const std::string &path);
    static void save(const Image &image, std::string &path);
};


