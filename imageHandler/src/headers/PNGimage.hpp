#pragma once

#include "image.hpp"
#include <vector>

#pragma pack(push, 1)

typedef struct {
    uint8_t bytes[8];  // 89 50 4E 47 0D 0A 1A 0A
    bool isValid() const {
        const uint8_t expected[8] = {137, 80, 78, 71, 13, 10, 26, 10};
        return std::equal(std::begin(bytes), std::end(bytes), expected);
    }
} PNGSignature;

typedef struct {
    uint32_t length;      
    char type[5];         
    uint8_t* data;        
    uint32_t crc;  

    void setTypeNullTerminated() {
        type[4] = '\0';
    }    
} PNGChunk;

typedef struct {
    uint32_t width;
    uint32_t height;
    uint8_t bit_depth;       
    uint8_t color_type;      
    uint8_t compression_method; 
    uint8_t filter_method;     
    uint8_t interlace_method;   
} PNGIHDR;

typedef struct {
    PNGSignature signature;
    PNGIHDR ihdr;
    uint8_t* raw_image_data;                   
    size_t raw_image_size;
    PNGChunk* chunks;         
    size_t chunk_count;
} PNGImageStruct;
#pragma pack(pop)


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
