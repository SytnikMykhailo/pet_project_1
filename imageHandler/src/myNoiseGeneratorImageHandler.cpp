#include <iostream>
#include <string>
#include <zlib.h>
#include <vector>
#include "headers/PNGimage.hpp"
#include "headers/imageloader.hpp"


int main() {
    std::cout << "Program started!" << std::endl;
    std::unique_ptr<Image> image = ImageLoader::load("../output/png1.png");

    std::cout << "Changing image!" << std::endl;
    for(int row = 43; row < 54; row++){
        for(int col = 32; col < 54; col++){
            image->setPixelColor(Color(24, 21, 43, 255), col, row);
        }
    }

    image->save("../output/png1.png");
    

    return 0;
}
