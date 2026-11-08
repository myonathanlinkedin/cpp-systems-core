#pragma once
#include "types.hpp"
#include <string>

namespace jitbitmap {

// Render a string onto a framebuffer at (originX,originY) using the supplied font.
// Returns true if the whole string fits inside the framebuffer; false if any part would start beyond the right edge.
bool renderText(const Font& font,
                const std::string& text,
                Framebuffer& fb,
                uint16_t originX,
                uint16_t originY);

} // namespace jitbitmap
