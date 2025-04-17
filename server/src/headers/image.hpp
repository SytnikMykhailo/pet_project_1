#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <vector>


#include "color.hpp"



class Image{
    public:
        virtual void setPixelColor(Color new_color, int pos_x, int pos_y) = 0;
        virtual Color getPixelColor(int pos_x, int pos_y) const = 0;
        virtual void save(const std::string &path_to_folder) const = 0;
};