#pragma once
#include <cstdint>
#include <vector>
#include <string>

namespace jitbitmap {

// Glyph representation: width, height, bitmap rows as vector<uint8_t> where each bit is a pixel.
struct Glyph {
    uint8_t width;
    uint8_t height;
    std::vector<uint8_t> rows; // each row packed LSB first, only needed bits.
    Glyph() = default;
    Glyph(uint8_t w, uint8_t h, const std::vector<uint8_t>& r) : width(w), height(h), rows(r) {};
};

// Font: mapping from char to Glyph.
struct Font {
    std::vector<Glyph> glyphs; // indexed by unsigned char (0‑255)
    Font() = default;
    explicit Font(const std::vector<Glyph>& g) : glyphs(g) {};
    const Glyph& get(char c) const {
        return glyphs[static_cast<unsigned char>(c)];
    }
};

// Monochrome framebuffer.
class Framebuffer {
public:
    const uint16_t width;
    const uint16_t height;
    std::vector<uint8_t> pixels; // row‑major, each pixel 0 or 1

    Framebuffer(uint16_t w, uint16_t h) : width(w), height(h), pixels(w * h, 0) {};

    void setPixel(uint16_t x, uint16_t y, uint8_t value) {
        if (x < width && y < height) {
            pixels[y * width + x] = value ? 1 : 0;
        }
    }

    uint8_t getPixel(uint16_t x, uint16_t y) const {
        if (x < width && y < height) {
            return pixels[y * width + x];
        }
        return 0;
    }

    bool equals(const Framebuffer& other) const {
        return width == other.width && height == other.height && pixels == other.pixels;
    }
};

} // namespace jitbitmap
