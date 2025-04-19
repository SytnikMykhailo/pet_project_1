#include <cstdint>

class Color{
    public:
        Color() = default;
        Color(uint8_t red, uint8_t green, uint8_t blue);
        ~Color();
    public:
        uint8_t getRed() const;
        uint8_t getGreen() const;
        uint8_t getBlue() const;
    private:
        uint8_t red = 0;
        uint8_t green = 0;
        uint8_t blue = 0;
};