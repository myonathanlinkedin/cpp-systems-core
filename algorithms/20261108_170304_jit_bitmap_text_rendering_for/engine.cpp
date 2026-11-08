#include <string>

#include "core.hpp"
#include <algorithm>

namespace jitbitmap {

bool renderText(const Font& font,
                const std::string& text,
                Framebuffer& fb,
                uint16_t originX,
                uint16_t originY) {
    uint16_t cursorX = originX;
    uint16_t cursorY = originY;
    bool fullyContained = true;

    for (char ch : text) {
        const Glyph& g = font.get(ch);

        // If the glyph would start beyond the right edge, stop rendering.
        if (cursorX + g.width > fb.width) {
            fullyContained = false;
            break;
        }

        // Draw glyph bitmap with clipping.
        for (uint8_t row = 0; row < g.height; ++row) {
            if (cursorY + row >= fb.height) {
                continue; // vertical clipping
            }
            uint8_t rowBits = g.rows[row];
            for (uint8_t col = 0; col < g.width; ++col) {
                if (cursorX + col >= fb.width) {
                    continue; // horizontal clipping inside glyph
                }
                uint8_t bit = (rowBits >> col) & 0x1;
                if (bit) {
                    fb.setPixel(cursorX + col, cursorY + row, 1);
                }
            }
        }

        // Advance cursor: one pixel spacing between characters.
        cursorX += g.width + 1;
    }

    return fullyContained;
}

} // namespace jitbitmap
