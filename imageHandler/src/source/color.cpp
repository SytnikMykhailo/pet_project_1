#include "../headers/color.hpp"


Color::Color(uint8_t _red, uint8_t _green, uint8_t _blue): 
    Color(_red, _green, _blue, 255)
    {

    } 

Color::Color(uint8_t _red, uint8_t _green, uint8_t _blue, uint8_t _alpha):
    red{_red}, green{_green}, blue{_blue}, alpha{_alpha}
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

uint8_t Color::getAlpha() const{
    return alpha;
}

Color::~Color() {

}
