#include <string>
#include <color.hpp>
#include <memory>

class Image{
    virtual void setPixelColor(Color new_color, int pos_x, int pos_y) = 0;
    virtual Color getPixelColor(int pos_x, int pos_y) const = 0;
    virtual void save(const std::string &path_to_folder) = 0;
};