#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace ac {

struct Match {
    int pattern_id;      // Index of the pattern in the original pattern list
    std::size_t position; // Position of the last character of the match in the text (0‑based)

    bool operator==(const Match& other) const noexcept {
        return pattern_id == other.pattern_id && position == other.position;
    };
};

} // namespace ac
