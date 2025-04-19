#include <iostream>
#include <string>
#include <zlib.h>
#include <vector>

int main() {
    std::cout << "Program started!" << std::endl;
    std::cout.flush();
    const std::string original = "Hello, Zlib! Zlib is great!";
    std::cout << "Original text: " << original << std::endl;

    uLong sourceLen = original.size();
    uLong destLen = compressBound(sourceLen);
    std::vector<Bytef> compressed(destLen);

    int compressionStatus = compress(compressed.data(), &destLen, 
                                     reinterpret_cast<const Bytef*>(original.data()), sourceLen);
    std::cerr << "Compression status: " << compressionStatus << ", Compressed size: " << destLen << std::endl;

    if (compressionStatus != Z_OK) {
        std::cerr << "Compression failed.\n";
        return 1;
    }

    

    return 0;
}
