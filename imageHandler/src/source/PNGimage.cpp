#include "../headers/PNGimage.hpp"
#include <zlib.h>
#include <stdexcept>
#include <cstring>
#include "../headers/PNGimage.hpp"
#include <winsock2.h>

void unfilterScanlines(std::vector<uint8_t>& image_data, uint32_t width, uint32_t height, int bpp);
uint8_t paethPredictor(uint8_t a, uint8_t b, uint8_t c);

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
    std::cout << "loading file " << path_to_image << " stared!" << std::endl;
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
    uint32_t width, height;
    std::cout << "loading file " << path_to_image << " continuing" << std::endl;
    while (in.read(reinterpret_cast<char*>(&chunk.length), sizeof(chunk.length))) {
        in.read(chunk.type, sizeof(chunk.type));
        chunk.setTypeNullTerminated();

        if (std::string(chunk.type) == "IHDR") {
            in.read(reinterpret_cast<char*>(&chunk.data), chunk.length);
            PNGIHDR ihdr;
            std::memcpy(&ihdr, chunk.data, sizeof(PNGIHDR));
            width = ihdr.width;
            height = ihdr.height;

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
    std::cout << "loading file " << path_to_image << " continuing" << std::endl;
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

void PNGImage::save(const std::string &path_to_file) const {
    std::ofstream out(path_to_file, std::ios::binary);
    if (!out) {
        std::cerr << "Failed to open file for writing: " << path_to_file << std::endl;
        return;
    }

    // PNG Signature
    uint8_t signature[8] = { 137, 80, 78, 71, 13, 10, 26, 10 };
    out.write(reinterpret_cast<char*>(signature), sizeof(signature));

    // Prepare IHDR chunk
    PNGIHDR ihdr;
    ihdr.width = htonl(width);
    ihdr.height = htonl(height);
    ihdr.bit_depth = 8;
    ihdr.color_type = 6;  // RGBA
    ihdr.compression_method = 0;
    ihdr.filter_method = 0;
    ihdr.interlace_method = 0;

    uint32_t ihdr_data_len = sizeof(PNGIHDR);
    uint32_t ihdr_len_be = htonl(ihdr_data_len);
    out.write(reinterpret_cast<char*>(&ihdr_len_be), sizeof(ihdr_len_be));

    const char ihdr_type[] = "IHDR";
    out.write(ihdr_type, 4);
    out.write(reinterpret_cast<char*>(&ihdr), ihdr_data_len);

    uint32_t ihdr_crc = crc32(0L, Z_NULL, 0);
    ihdr_crc = crc32(ihdr_crc, reinterpret_cast<const Bytef*>(ihdr_type), 4);
    ihdr_crc = crc32(ihdr_crc, reinterpret_cast<const Bytef*>(&ihdr), ihdr_data_len);
    uint32_t ihdr_crc_be = htonl(ihdr_crc);
    out.write(reinterpret_cast<char*>(&ihdr_crc_be), sizeof(ihdr_crc_be));

    // Prepare raw image data with filter bytes
    std::vector<uint8_t> raw_data;
    for (int y = 0; y < height; ++y) {
        raw_data.push_back(0); // No filter
        for (int x = 0; x < width; ++x) {
            const Color& c = pixels[y * width + x];
            raw_data.push_back(c.getRed());
            raw_data.push_back(c.getGreen());
            raw_data.push_back(c.getBlue());
            raw_data.push_back(c.getAlpha());
        }
    }

    // Compress image data
    uLongf compressed_size = compressBound(raw_data.size());
    std::vector<uint8_t> compressed_data(compressed_size);
    if (compress(compressed_data.data(), &compressed_size, raw_data.data(), raw_data.size()) != Z_OK) {
        std::cerr << "Compression failed.\n";
        return;
    }

    uint32_t idat_len_be = htonl(static_cast<uint32_t>(compressed_size));
    out.write(reinterpret_cast<char*>(&idat_len_be), sizeof(idat_len_be));

    const char idat_type[] = "IDAT";
    out.write(idat_type, 4);
    out.write(reinterpret_cast<char*>(compressed_data.data()), compressed_size);

    uint32_t idat_crc = crc32(0L, Z_NULL, 0);
    idat_crc = crc32(idat_crc, reinterpret_cast<const Bytef*>(idat_type), 4);
    idat_crc = crc32(idat_crc, compressed_data.data(), compressed_size);
    uint32_t idat_crc_be = htonl(idat_crc);
    out.write(reinterpret_cast<char*>(&idat_crc_be), sizeof(idat_crc_be));

    // IEND chunk
    uint32_t iend_len_be = htonl(0);
    out.write(reinterpret_cast<char*>(&iend_len_be), sizeof(iend_len_be));

    const char iend_type[] = "IEND";
    out.write(iend_type, 4);

    uint32_t iend_crc = crc32(0L, Z_NULL, 0);
    iend_crc = crc32(iend_crc, reinterpret_cast<const Bytef*>(iend_type), 4);
    uint32_t iend_crc_be = htonl(iend_crc);
    out.write(reinterpret_cast<char*>(&iend_crc_be), sizeof(iend_crc_be));

    out.close();
    std::cout << "Saved PNG image to " << path_to_file << std::endl;
}

PNGImage::~PNGImage(){
    
}

PNGImage::PNGImage(int _height, int _width):
    height{_height}, width{_width}
{
    pixels.resize(width * height);
}