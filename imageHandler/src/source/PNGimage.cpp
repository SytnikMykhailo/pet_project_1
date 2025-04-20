#include "../headers/PNGimage.hpp"
#include <zlib.h>
#include <stdexcept>
#include <cstring>
#include "PNGImage.hpp"

void unfilterScanlines(std::vector<uint8_t>& image_data, uint32_t width, uint32_t height, int bpp) {
    const int stride = width * bpp;

    for (uint32_t y = 0; y < height; ++y) {
        uint8_t* row = &image_data[y * (stride + 1)];
        uint8_t filter_type = row[0];
        uint8_t* curr = row + 1;
        uint8_t* prev = y == 0 ? nullptr : &image_data[(y - 1) * (stride + 1) + 1];

        switch (filter_type) {
            case 0: break;

            case 1:
                for (int i = bpp; i < stride; ++i)
                    curr[i] += curr[i - bpp];
                break;

            case 2:
                if (!prev) break;
                for (int i = 0; i < stride; ++i)
                    curr[i] += prev[i];
                break;

            case 3:
                for (int i = 0; i < stride; ++i) {
                    uint8_t left = (i >= bpp) ? curr[i - bpp] : 0;
                    uint8_t above = (prev) ? prev[i] : 0;
                    curr[i] += (left + above) / 2;
                }
                break;

            case 4:
                for (int i = 0; i < stride; ++i) {
                    uint8_t left = (i >= bpp) ? curr[i - bpp] : 0;
                    uint8_t above = (prev) ? prev[i] : 0;
                    uint8_t upper_left = (prev && i >= bpp) ? prev[i - bpp] : 0;
                    curr[i] += paethPredictor(left, above, upper_left);
                }
                break;

            default:
                throw std::runtime_error("Unknown PNG filter type");
        }
    }
}

uint8_t paethPredictor(uint8_t a, uint8_t b, uint8_t c) {
    int p = a + b - c;
    int pa = std::abs(p - a);
    int pb = std::abs(p - b);
    int pc = std::abs(p - c);
    if (pa <= pb && pa <= pc) return a;
    if (pb <= pc) return b;
    return c;
}

std::unique_ptr<Image> PNGImage::load(const std::string &path_to_image) {
    std::ifstream in(path_to_image, std::ios::binary);
    if (!in) {
        std::cerr << "Failed to open image: " << path_to_image << std::endl;
        return nullptr;
    }

    PNGSignature signature;
    in.read(reinterpret_cast<char*>(&signature), sizeof(signature));
    if (!signature.isValid()) {
        std::cerr << "Invalid PNG signature." << std::endl;
        return nullptr;
    }

    PNGChunk chunk;
    std::vector<uint8_t> image_data;
    uint32_t width, height, bit_depth, color_type;

    while (in.read(reinterpret_cast<char*>(&chunk.length), sizeof(chunk.length))) {
        in.read(chunk.type, sizeof(chunk.type));
        chunk.setTypeNullTerminated();

        if (std::string(chunk.type) == "IHDR") {
            in.read(reinterpret_cast<char*>(&chunk.data), chunk.length);
            PNGIHDR ihdr;
            std::memcpy(&ihdr, chunk.data, sizeof(PNGIHDR));
            width = ihdr.width;
            height = ihdr.height;
            bit_depth = ihdr.bit_depth;
            color_type = ihdr.color_type;

            image_data.resize(width * height * 4);

            in.ignore(4);
        } else if (std::string(chunk.type) == "IDAT") {
            std::vector<uint8_t> compressed_data(chunk.length);
            in.read(reinterpret_cast<char*>(compressed_data.data()), chunk.length);

            uLongf decompressed_size = width * height * 4;
            std::vector<uint8_t> decompressed_data(decompressed_size);

            int result = uncompress(decompressed_data.data(), &decompressed_size, compressed_data.data(), chunk.length);
            if (result != Z_OK) {
                std::cerr << "Failed to decompress IDAT chunk.\n";
                return nullptr;
            }

            image_data.insert(image_data.end(), decompressed_data.begin(), decompressed_data.end());
        } else {
            in.ignore(chunk.length + 4);
        }
        if (std::string(chunk.type) == "IEND") {
            break;
        }
    }
    in.close();

    unfilterScanlines(image_data, width, height, 4);

    auto png_image = std::make_unique<PNGImage>(height, width);
    for (uint32_t y = 0; y < height; ++y) {
        for (uint32_t x = 0; x < width; ++x) {
            uint8_t r = image_data[(y * width + x) * 4 + 0];
            uint8_t g = image_data[(y * width + x) * 4 + 1];
            uint8_t b = image_data[(y * width + x) * 4 + 2];
            uint8_t a = image_data[(y * width + x) * 4 + 3];
            png_image->setPixelColor(Color(r, g, b, a), x, y);
        }
    }

    return png_image;
}


void PNGImage::setPixelColor(Color new_color, int pos_x, int pos_y) {
    if (pos_x >= 0 && pos_x < width && pos_y >= 0 && pos_y < height) {
        pixels[pos_y * width + pos_x] = new_color;
    } else {
        std::cerr << "setPixelColor: position out of bounds\n";
    }
}

Color PNGImage::getPixelColor(int pos_x, int pos_y) const {
    if (pos_x >= 0 && pos_x < width && pos_y >= 0 && pos_y < height) {
        return pixels[pos_y * width + pos_x];
    } else {
        std::cerr << "getPixelColor: position out of bounds\n";
        return Color();
    }
}

void PNGImage::save(const std::string &path_to_folder) const {
    std::cout << path_to_folder << std::endl;
}

PNGImage::~PNGImage(){
    
}

PNGImage::PNGImage(int _height, int _width):
    height{_height}, width{_width}
{
    pixels.resize(width * height);
}

uint32_t readBigEndianUint32(std::istream& stream) {
    uint8_t bytes[4];
    stream.read(reinterpret_cast<char*>(bytes), 4);
    return (bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | (bytes[3]);
}