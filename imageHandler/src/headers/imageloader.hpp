#include "BMPimage.hpp"
#include "JPEGimage.hpp"
#include "PNGimage.hpp"
#include <stdexcept>

class ImageLoader{
    public:
        static std::unique_ptr<Image> load(const std::string &path);
        static void save(const Image &image, std::string &path);
};


