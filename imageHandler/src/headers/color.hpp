#include <cstdint>

class Color{
    public:
        Color() = default;
        Color(uint8_t _red, uint8_t _green, uint8_t _blue);
        Color(uint8_t _red, uint8_t _green, uint8_t _blue, uint8_t _alpha);
        ~Color();
    public:
        uint8_t getRed() const;
        uint8_t getGreen() const;
        uint8_t getBlue() const;
        uint8_t getAlpha() const;
    private:
        uint8_t red = 0;
        uint8_t green = 0;
        uint8_t blue = 0;
        uint8_t alpha = 0;
};