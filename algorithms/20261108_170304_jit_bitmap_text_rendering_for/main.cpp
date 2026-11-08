#include <vector>

#include "types.hpp"
#include "core.hpp"
#include <cassert>
#include <iostream>

using namespace jitbitmap;

void testSingleGlyphRendering() {
    // 5×7 glyph for 'A'.
    std::vector<uint8_t> aRows = {
        0b00100,
        0b01010,
        0b10001,
        0b11111,
        0b10001,
        0b10001,
        0b10001
    };
    Glyph glyphA(5, 7, aRows);
    Font font(std::vector<Glyph>(256));
    font.glyphs['A'] = glyphA;

    Framebuffer fb(10, 10);
    bool ok = renderText(font, "A", fb, 2, 1);
    assert(ok);

    Framebuffer expected(10, 10);
    for (uint8_t r = 0; r < 7; ++r) {
        uint8_t bits = aRows[r];
        for (uint8_t c = 0; c < 5; ++c) {
            if ((bits >> c) & 0x1) {
                expected.setPixel(2 + c, 1 + r, 1);
            }
        }
    }
    assert(fb.equals(expected));
}

void testStringClipping() {
    // 3×3 block glyph for 'X'.
    std::vector<uint8_t> xRows = {
        0b111,
        0b101,
        0b111
    };
    Glyph glyphX(3, 3, xRows);
    Font font(std::vector<Glyph>(256));
    font.glyphs['X'] = glyphX;

    Framebuffer fb(8, 5);
    bool fully = renderText(font, "XXX", fb, 2, 0);
    assert(!fully); // third glyph would start beyond right edge.

    Framebuffer expected(8, 5);
    // First glyph at x=2.
    for (uint8_t r = 0; r < 3; ++r) {
        uint8_t bits = xRows[r];
        for (uint8_t c = 0; c < 3; ++c) {
            if ((bits >> c) & 0x1) {
                expected.setPixel(2 + c, r, 1);
            }
        }
    }
    // Second glyph at x=2+3+1=6 (partial clipping on column 8).
    for (uint8_t r = 0; r < 3; ++r) {
        uint8_t bits = xRows[r];
        for (uint8_t c = 0; c < 3; ++c) {
            uint16_t xPos = 6 + c;
            if (xPos < 8 && ((bits >> c) & 0x1)) {
                expected.setPixel(xPos, r, 1);
            }
        }
    }
    assert(fb.equals(expected));
}

int main() {
    testSingleGlyphRendering();
    testStringClipping();
    std::cout << "All JIT bitmap rendering tests passed.\n";
    return 0;
}
