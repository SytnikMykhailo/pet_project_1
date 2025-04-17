#include "../headers/color.hpp"


Color::Color(int _red, int _green, int _blue): 
    red{_red}, green{_green}, blue{_blue}
    {

    }

int Color::getRed() const{
    return red;
}

int Color::getGreen() const{
    return green;
}

int Color::getBlue() const{
    return blue;
}

Color::~Color() {

}
