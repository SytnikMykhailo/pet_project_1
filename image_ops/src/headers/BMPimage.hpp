#pragma once

#include "image.hpp"


#pragma pack(push, 1)
struct BMPFileHeader {
    uint16_t fileType{0x4D42};
    uint32_t fileSize{0};
    uint16_t reserved1{0};
    uint16_t reserved2{0};
    uint32_t offsetData{54};
};

struct BMPInfoHeader {
    uint32_t size{40};
    int32_t width{0};
    int32_t height{0};
    uint16_t planes{1};
    uint16_t bitCount{24};
    uint32_t compression{0};
    uint32_t imageSize{0};
    int32_t xPixelsPerMeter{0};
    int32_t yPixelsPerMeter{0};
    uint32_t colorsUsed{0};
    uint32_t colorsImportant{0};
};
#pragma pack(pop)


class BMPImage: public Image{
    public:
        BMPIMage() = delete;
        BMPImage(uint8_t _height, uint8_t _width);
        ~BMPImage();
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
