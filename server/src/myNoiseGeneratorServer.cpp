#include <iostream>
#include "headers/BMPImage.hpp"

int main(int argc, char **argv){
    std::cout << "Hello, World!" << std::endl;
    std::cout << argv[argc - 1] << std::endl;
    uint8_t height = 124;
    uint8_t width = 124;
    Image *bmp_image = new BMPImage(height, width);
    /*for(int row = 0; row < 124; row++){
        for(int col = 0; col < 124; col++){
            bmp_image->setPixelColor(Color(122, 226, 207), col, row);
        }
    }*/
    bmp_image->save("../output/test_save_1.bmp");
    return 0;
}