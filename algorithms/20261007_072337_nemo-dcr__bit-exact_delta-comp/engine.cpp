#include "types.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace nemo_dcr {

// Zig‑zag encode a signed 32‑bit integer to an unsigned 32‑bit integer.
static inline std::uint32_t zigzagEncode(std::int32_t v) noexcept {
    return (static_cast<std::uint32_t>(v) << 1) ^ static_cast<std::uint32_t>(v >> 31);
}

// Zig‑zag decode.
static inline std::int32_t zigzagDecode(std::uint32_t v) noexcept {
    return static_cast<std::int32_t>((v >> 1) ^ static_cast<std::uint32_t>(-(static_cast<int>(v & 1))));
}

// Encode a single unsigned integer using LEB128 (little‑endian base 128).
static void leb128Encode(std::uint32_t value, Delta& out) {
    do {
        std::uint8_t byte = static_cast<std::uint8_t>(value & 0x7F);
        value >>= 7;
        if (value != 0) {
            byte |= 0x80; // more bytes follow
        }
        out.push_back(byte);
    } while (value != 0);
}

// Decode a LEB128‑encoded unsigned integer. Returns the decoded value and the number
// of bytes consumed via the reference parameter `consumed`.
static std::uint32_t leb128Decode(const Delta& in, std::size_t& offset, std::size_t& consumed) {
    std::uint32_t result = 0;
    std::size_t shift = 0;
    consumed = 0;
    while (offset < in.size()) {
        std::uint8_t byte = in[offset++];
        ++consumed;
        result |= static_cast<std::uint32_t>(byte & 0x7F) << shift;
        if ((byte & 0x80) == 0) {
            break;
        }
        shift += 7;
    }
    return result;
}

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

Delta computeDelta(const Model& base, const Model& updated) {
    assert(base.size() == updated.size() && "Base and updated models must have equal length");
    std::vector<std::int32_t> rawDelta;
    rawDelta.reserve(base.size());

    for (std::size_t i = 0; i < base.size(); ++i) {
        // Compute signed difference; cast to signed to allow negative deltas.
        std::int32_t diff = static_cast<std::int32_t>(updated[i]) - static_cast<std::int32_t>(base[i]);
        rawDelta.push_back(diff);
    }
    return encodeDelta(rawDelta);
}

Model applyDelta(const Model& base, const Delta& delta) {
    std::vector<std::int32_t> rawDelta = decodeDelta(delta);
    assert(base.size() == rawDelta.size() && "Delta length must match base model length");
    Model result;
    result.reserve(base.size());

    for (std::size_t i = 0; i < base.size(); ++i) {
        std::int64_t reconstructed = static_cast<std::int64_t>(base[i]) + rawDelta[i];
        // Clamp to 32‑bit unsigned range (wrap‑around semantics of uint32_t).
        result.push_back(static_cast<std::uint32_t>(reconstructed));
    }
    return result;
}

// Encode a vector of signed 32‑bit deltas into a byte stream using zig‑zag + LEB128.
Delta encodeDelta(const std::vector<std::int32_t>& rawDelta) {
    Delta out;
    out.reserve(rawDelta.size() * 5); // worst case: 5 bytes per int32_t

    for (std::int32_t v : rawDelta) {
        std::uint32_t zz = zigzagEncode(v);
        leb128Encode(zz, out);
    }
    return out;
}

// Decode a byte stream back into a vector of signed 32‑bit deltas.
std::vector<std::int32_t> decodeDelta(const Delta& encoded) {
    std::vector<std::int32_t> out;
    out.reserve(encoded.size() / 2); // heuristic

    std::size_t offset = 0;
    while (offset < encoded.size()) {
        std::size_t consumed = 0;
        std::uint32_t zz = leb128Decode(encoded, offset, consumed);
        std::int32_t value = zigzagDecode(zz);
        out.push_back(value);
    }
    return out;
}

} // namespace nemo_dcr
