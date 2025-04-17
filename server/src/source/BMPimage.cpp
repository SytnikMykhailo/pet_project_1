#include "../headers/BMPimage.hpp"

BMPImage::BMPImage(int _height, int _width):
    height{_height}, width{_width}
{
    pixels.resize(width * height);
}

std::unique_ptr<Image> BMPImage::load(const std::string &path_to_image){
    std::ifstream in(path_to_image, std::ios::binary);
    if (!in) {
        std::cerr << "Failed to open image: " << path_to_image << "\n";
        return nullptr;
    }

    BMPFileHeader bmpFileHeader;
    BMPInfoHeader bmpInfoHeader;

    in.read(reinterpret_cast<char*>(&bmpFileHeader), sizeof(bmpFileHeader));
    in.read(reinterpret_cast<char*>(&bmpInfoHeader), sizeof(bmpInfoHeader));

    if(bmpFileHeader.fileType != 0x4D42){
        std::cerr << "Unsupported BMP format.\n";
        return nullptr;
    }

    int width = bmpInfoHeader.width;
    int height = bmpInfoHeader.height;
    int rowStride = (width * 3 + 3) & ~3;
    int paddingSize = rowStride - width * 3;

    auto image = std::make_unique<BMPImage>(height, width);

    in.seekg(bmpFileHeader.offsetData, std::ios::beg);

    for (int row = height - 1; row >= 0; row--) {
        for (int col = 0; col < width; col++) {
            uint8_t b = in.get();
            uint8_t g = in.get();
            uint8_t r = in.get();
            image->setPixelColor(Color{r, g, b}, col, row);
        }
        in.ignore(paddingSize);
    }

    in.close();
    return image;
}   

void BMPImage::setPixelColor(Color new_color, int pos_x, int pos_y) {
    if (pos_x >= 0 && pos_x < width && pos_y >= 0 && pos_y < height) {
        pixels[pos_y * width + pos_x] = new_color;
    } else {
        std::cerr << "setPixelColor: position out of bounds\n";
    }
}

Color BMPImage::getPixelColor(int pos_x, int pos_y) const {
    if (pos_x >= 0 && pos_x < width && pos_y >= 0 && pos_y < height) {
        return pixels[pos_y * width + pos_x];
    } else {
        std::cerr << "getPixelColor: position out of bounds\n";
        return Color();
    }
}

void BMPImage::save(const std::string &path_to_folder) const {
    std::ofstream out(path_to_folder, std::ios::binary);
    if(!out){
        std::cerr << "Failed to save file" << std::endl;
        return;
    }


    BMPFileHeader bmpFileHeader;
    BMPInfoHeader bmpInfoHeader;

    int rowStride = (width * 3 + 3) & ~3;
    int paddingSize = rowStride - width * 3;
    int fileSize = 54 + rowStride * height;

    bmpFileHeader.fileSize = fileSize;
    bmpFileHeader.offsetData = 54;

    bmpInfoHeader.width = width;
    bmpInfoHeader.height = height;
    bmpInfoHeader.imageSize = rowStride * height;

    out.write(reinterpret_cast<const char*>(&bmpFileHeader), sizeof(bmpFileHeader));
    out.write(reinterpret_cast<const char*>(&bmpInfoHeader), sizeof(bmpInfoHeader));

    for(int row = height - 1; row >= 0; row--) {
        for(int col = 0; col < width; col++) {
            Color c = getPixelColor(col, row);
            out.put(static_cast<uint8_t>(c.getBlue()));
            out.put(static_cast<uint8_t>(c.getGreen()));
            out.put(static_cast<uint8_t>(c.getRed()));
        }

        for (int i = 0; i < paddingSize; ++i) {
            out.put(0);
        }
    }
}

BMPImage::~BMPImage(){
    
}
