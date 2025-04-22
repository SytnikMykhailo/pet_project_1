#include "../headers/PNGimage.hpp"
#include <zlib.h>
#include <stdexcept>
#include <cstring>
#include "../headers/PNGimage.hpp"
#include <winsock2.h>

void unfilterScanlines(std::vector<uint8_t>& image_data, uint32_t width, uint32_t height, int bpp);
uint8_t paethPredictor(uint8_t a, uint8_t b, uint8_t c);

void unfilterScanlines(std::vector<uint8_t>& data, uint32_t width, uint32_t height, int bpp) {
    const int stride = width * bpp;

    for (uint32_t y = 0; y < height; ++y) {
        std::cout << "unfilterScanlines" << std::endl;
        uint8_t* row = &data[y * (stride + 1)];
        uint8_t filter_type = row[0];
        uint8_t* curr = row + 1;
        uint8_t* prev = y == 0 ? nullptr : &data[(y - 1) * (stride + 1) + 1];
        std::cout << filter_type << std::endl;
        switch (filter_type) {
            case 0:
                std::cout << "case 0" << std::endl;
                break;
            case 1:
                std::cout << "case 1" << std::endl;
                for (int i = bpp; i < stride; ++i)
                    curr[i] = static_cast<uint8_t>(int(curr[i]) + int(curr[i - bpp]));
                break;
            case 2:
                std::cout << "case 2" << std::endl;
                if (!prev) break;
                for (int i = 0; i < stride; ++i)
                    curr[i] = static_cast<uint8_t>(int(curr[i]) + int(prev[i]));
                break;
            case 3:
                std::cout << "case 3" << std::endl;
                for (int i = 0; i < stride; ++i) {
                    int left = (i >= bpp) ? int(curr[i - bpp]) : 0;
                    int above = (prev) ? int(prev[i]) : 0;
                    curr[i] = static_cast<uint8_t>(int(curr[i]) + ((left + above) / 2));
                }
                break;
            case 4:
                std::cout << "case 4" << std::endl;
                for (int i = 0; i < stride; ++i) {
                    int left = (i >= bpp) ? int(curr[i - bpp]) : 0;
                    int above = (prev) ? int(prev[i]) : 0;
                    int upper_left = (prev && i >= bpp) ? int(prev[i - bpp]) : 0;
                    curr[i] = static_cast<uint8_t>(int(curr[i]) + paethPredictor(left, above, upper_left));
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
    std::vector<uint8_t> compressed_data;
    while (in.read(reinterpret_cast<char*>(&chunk.length), sizeof(chunk.length))) {
        chunk.length = htonl(chunk.length);
        std::cout << "aboba" << std::endl;
        in.read(chunk.type, sizeof(chunk.type) - 1);
        chunk.setTypeNullTerminated();

        std::cout << "aboba" << std::endl;
        if (std::string(chunk.type) == "IHDR") {
            std::cout << "IHDR" << std::endl;
            chunk.data.resize(chunk.length);
            in.read(reinterpret_cast<char*>(chunk.data.data()), chunk.length);
            std::cout << "IHDR" << chunk.length << std::endl;
            PNGIHDR ihdr;
            
            if (chunk.data.size() < sizeof(PNGIHDR)) {
                std::cerr << "chunk.length is too small!" << std::endl;
                return nullptr;
            }
            std::cout << "IHDR" << sizeof(ihdr) << "asdf" << sizeof(chunk.data) << std::endl;
            std::memcpy(&ihdr, chunk.data.data(), sizeof(PNGIHDR));
            std::cout << "IHDR" << std::endl;
            width = htonl(ihdr.width);
            height = htonl(ihdr.height);
            std::cout << "IHDR" << std::endl;
            image_data.resize(width * height * 4);

            in.ignore(4);
            std::cout << "aboba" << std::endl;
        } else if (std::string(chunk.type) == "IDAT") {
            std::vector<uint8_t> part(chunk.length);
            in.read(reinterpret_cast<char*>(part.data()), chunk.length);
            compressed_data.insert(compressed_data.end(), part.begin(), part.end());
        } else {
            std::cout << "aboba" << std::endl;
            in.ignore(chunk.length + 4);
        }
        std::cout << "aboba" << std::endl;
        if (std::string(chunk.type) == "IEND") {
            std::cout << "aboba" << std::endl;
            break;
        }
    }
    uLongf decompressed_size = (width * 4 + 1) * height; 
    std::vector<uint8_t> decompressed_data(decompressed_size);

    int result = uncompress(decompressed_data.data(), &decompressed_size,
                            compressed_data.data(), compressed_data.size());

    if (result != Z_OK) {
        std::cerr << "Failed to decompress combined IDAT data. Error code: " << result << std::endl;
        return nullptr;
    }
    in.close();
    std::cout << "Compressed data size: " << compressed_data.size() << std::endl;
    std::cout << "Decompressed expected size: " << decompressed_size << std::endl;
    std::cout << "dimesions: " << height << " * " << width << std::endl;
    std::cout << "loading file " << path_to_image << " continuing" << std::endl;
    unfilterScanlines(decompressed_data, width, height, 4);
    std::cout << "loading file " << path_to_image << " continuing" << std::endl;
    auto png_image = std::make_unique<PNGImage>(height, width);
    for (uint32_t y = 0; y < height; ++y) {
        for (uint32_t x = 0; x < width; ++x) {
            size_t i = y * (width * 4 + 1) + 1 + x * 4;
            uint8_t r = decompressed_data[i + 0];
            uint8_t g = decompressed_data[i + 1];
            uint8_t b = decompressed_data[i + 2];
            uint8_t a = decompressed_data[i + 3];
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