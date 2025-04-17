class Color{
    public:
        Color() = default;
        Color(int red, int green, int blue);
        ~Color();
    public:
        int getRed() const;
        int getGreen() const;
        int getBlue() const;
    private:
        int red = 255;
        int green = 255;
        int blue = 255;
};