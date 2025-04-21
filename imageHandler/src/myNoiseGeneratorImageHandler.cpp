#include <iostream>
#include <string>
#include <zlib.h>
#include <vector>
#include "headers/PNGimage.hpp"
#include "headers/imageloader.hpp"


int main() {
    std::cout << "Program started!" << std::endl;
    std::unique_ptr<Image> image = ImageLoader::load("../output/png1.png");


    for(int row = 0; row < 54; row++){
        for(int col = 0; col < 45; col++){
            image->setPixelColor(Color(23, 34, 234, 255), col, row);
        }
    }

    image->save("../output/png1.png");
    

    return 0;
}
