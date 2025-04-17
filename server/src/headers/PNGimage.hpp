#pragma once

#include "image.hpp"
#include <vector>

class PNGImage: public Image{
    public:
        PNGImage(int _height, int _width);
        ~PNGImage();
    private:
        int height;
        int width;
       
        std::vector<Color> pixels; 
    public:
        void setPixelColor(Color new_color, int pos_x, int pos_y) override;
        Color getPixelColor(int pos_x, int pos_y) const override;
        void save(const std::string &path_to_folder) const override;

    public:
        static std::unique_ptr<Image> load(const std::string &path_to_image);
};
