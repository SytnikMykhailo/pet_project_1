#pragma once
#include <cmath>
#include <vector>
#include <random>
#include <algorithm>

struct Vector2 {
    double x, y;
    Vector2(double x_, double y_) : x(x_), y(y_) {}
    static double dot(const Vector2& v1, const Vector2& v2) {
        return v1.x * v2.x + v1.y * v2.y;
    }
};

namespace VectorOperations {
    inline double fade(double t) {
        return ((6 * t - 15) * t + 10) * t * t * t;
    }

    inline double lerp(double t, double a, double b) {
        return a + t * (b - a);
    }

    inline Vector2 getConstantVector(int permutationTableValue) {
        int v = permutationTableValue & 3;
        switch (v) {
            case 0: return Vector2(1.0, 1.0);
            case 1: return Vector2(-1.0, 1.0);
            case 2: return Vector2(-1.0, -1.0);
            default: return Vector2(1.0, -1.0);
        }
    }
}

class PerlinNoise2D {
public:
    static double noise2D(double x, double y) {
        static std::vector<int> Permutation = makePermutation();
        int X = static_cast<int>(std::floor(x)) & 255;
        int Y = static_cast<int>(std::floor(y)) & 255;

        double xf = x - std::floor(x);
        double yf = y - std::floor(y);

        Vector2 topRight(xf - 1.0, yf - 1.0);
        Vector2 topLeft(xf, yf - 1.0);
        Vector2 bottomRight(xf - 1.0, yf);
        Vector2 bottomLeft(xf, yf);

        int valueTopRight = Permutation[Permutation[X + 1] + Y + 1];
        int valueTopLeft = Permutation[Permutation[X] + Y + 1];
        int valueBottomRight = Permutation[Permutation[X + 1] + Y];
        int valueBottomLeft = Permutation[Permutation[X] + Y];

        double dotTopRight = Vector2::dot(topRight, VectorOperations::getConstantVector(valueTopRight));
        double dotTopLeft = Vector2::dot(topLeft, VectorOperations::getConstantVector(valueTopLeft));
        double dotBottomRight = Vector2::dot(bottomRight, VectorOperations::getConstantVector(valueBottomRight));
        double dotBottomLeft = Vector2::dot(bottomLeft, VectorOperations::getConstantVector(valueBottomLeft));

        double u = VectorOperations::fade(xf);
        double v = VectorOperations::fade(yf);

        double lerpBottom = VectorOperations::lerp(u, dotBottomLeft, dotBottomRight);
        double lerpTop = VectorOperations::lerp(u, dotTopLeft, dotTopRight);

        return VectorOperations::lerp(v, lerpBottom, lerpTop);
    }

    static double fractalBrownianMotion(double x, double y, int numberOfOctaves, double frequency) {
        double result = 0.0;
        double amplitude = 4.0;
        for (int octave = 0; octave < numberOfOctaves; ++octave) {
            result += amplitude * noise2D(x * frequency, y * frequency);
            amplitude *= 0.5;
            frequency *= 2.0;
        }
        return result;
    }

private:
    static void shuffle(std::vector<int>& arrayToShuffle) {
        static std::random_device rd;
        static std::mt19937 g(rd());
        std::shuffle(arrayToShuffle.begin(), arrayToShuffle.end(), g);
    }

    static std::vector<int> makePermutation() {
        std::vector<int> p(256);
        for (int i = 0; i < 256; ++i) p[i] = i;
        shuffle(p);
        std::vector<int> permutation(512);
        for (int i = 0; i < 256; ++i) {
            permutation[i] = p[i];
            permutation[i + 256] = p[i];
        }
        return permutation;
    }
};