#include "../headers/color.hpp"


Color::Color(uint8_t _red, uint8_t _green, uint8_t _blue): 
    red{_red}, green{_green}, blue{_blue}
    {

    }

uint8_t Color::getRed() const{
    return red;
}

uint8_t Color::getGreen() const{
    return green;
}

uint8_t Color::getBlue() const{
    return blue;
}

Color::~Color() {

}
