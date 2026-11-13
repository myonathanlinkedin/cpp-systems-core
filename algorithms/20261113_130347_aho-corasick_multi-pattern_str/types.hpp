#pragma once
#include <cstddef>

namespace ac {

struct Match {
    int patternId;
    std::size_t position; // index of the last character of the match (0‑based)
};

} // namespace ac
