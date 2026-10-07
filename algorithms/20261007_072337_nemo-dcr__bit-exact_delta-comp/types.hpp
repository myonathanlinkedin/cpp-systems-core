#pragma once

#include <cstdint>
#include <vector>

// Model representation: a sequence of 32‑bit unsigned parameters.
using Model = std::vector<std::uint32_t>;

// Delta representation: byte stream produced by variable‑length encoding.
using Delta = std::vector<std::uint8_t>;

// ---------------------------------------------------------------------------
// Engine API (implemented in engine.cpp)
// ---------------------------------------------------------------------------
namespace nemo_dcr {

// Compute a delta (byte stream) that transforms `base` into `updated`.
// The delta is encoded as a sequence of zig‑zag LEB128 signed integers.
Delta computeDelta(const Model& base, const Model& updated);

// Apply a previously computed delta to `base` and return the reconstructed model.
Model applyDelta(const Model& base, const Delta& delta);

// Helper: compress a raw delta (signed int32_t vector) into a byte stream.
Delta encodeDelta(const std::vector<std::int32_t>& rawDelta);

// Helper: decode a byte stream back into a raw delta vector.
std::vector<std::int32_t> decodeDelta(const Delta& encoded);

} // namespace nemo_dcr
