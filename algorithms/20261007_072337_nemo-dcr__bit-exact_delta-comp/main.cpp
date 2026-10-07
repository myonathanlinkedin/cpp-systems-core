#include <vector>

#include "types.hpp"
#include "engine.cpp"   // Pull in implementation for a single‑translation‑unit build

#include <cassert>
#include <iostream>
#include <random>

int main() {
    using namespace nemo_dcr;

    // Helper to generate a random model of given size.
    auto generateModel = [](std::size_t size, std::uint32_t maxVal = 0xFFFFFFFFu) {
        Model m;
        m.reserve(size);
        std::mt19937 rng{12345};
        std::uniform_int_distribution<std::uint32_t> dist(0, maxVal);
        for (std::size_t i = 0; i < size; ++i) {
            m.push_back(dist(rng));
        }
        return m;
    };

    // Edge case: empty model.
    {
        Model base;
        Model updated;
        Delta d = computeDelta(base, updated);
        Model reconstructed = applyDelta(base, d);
        assert(reconstructed == updated && "Empty model round‑trip failed");
    }

    // Small deterministic test.
    {
        Model base = {10, 20, 30, 40};
        Model updated = {12, 18, 30, 45};
        Delta d = computeDelta(base, updated);
        Model round = applyDelta(base, d);
        assert(round == updated && "Deterministic delta test failed");
    }

    // Randomized stress test.
    {
        const std::size_t modelSize = 1024;
        Model base = generateModel(modelSize);
        Model updated = base; // copy

        // Apply small random perturbations.
        std::mt19937 rng{98765};
        std::uniform_int_distribution<int> deltaDist(-5, 5);
        for (auto& v : updated) {
            int delta = deltaDist(rng);
            std::int64_t tmp = static_cast<std::int64_t>(v) + delta;
            v = static_cast<std::uint32_t>(tmp);
        }

        Delta d = computeDelta(base, updated);
        Model round = applyDelta(base, d);
        assert(round == updated && "Randomized delta round‑trip failed");

        // Verify that encode/decode are inverse operations.
        std::vector<std::int32_t> raw = decodeDelta(d);
        Delta reencoded = encodeDelta(raw);
        assert(d == reencoded && "Encode/Decode symmetry failed");
    }

    // Verify that compression is size‑effective for sparse changes.
    {
        const std::size_t modelSize = 10000;
        Model base = generateModel(modelSize);
        Model updated = base;

        // Change only 1% of entries.
        std::mt19937 rng{55555};
        std::uniform_int_distribution<std::size_t> idxDist(0, modelSize - 1);
        std::uniform_int_distribution<int> deltaDist(-1000, 1000);
        for (std::size_t i = 0; i < modelSize / 100; ++i) {
            std::size_t idx = idxDist(rng);
            int delta = deltaDist(rng);
            std::int64_t tmp = static_cast<std::int64_t>(updated[idx]) + delta;
            updated[idx] = static_cast<std::uint32_t>(tmp);
        }

        Delta d = computeDelta(base, updated);
        // Expect delta size to be significantly smaller than raw model size (4 * N bytes).
        std::cout << "Raw model size: " << base.size() * sizeof(std::uint32_t) << " bytes\n";
        std::cout << "Delta size:     " << d.size() << " bytes\n";
        assert(d.size() < base.size() * sizeof(std::uint32_t) && "Delta not smaller than raw model");
    }

    std::cout << "All tests passed.\n";
    return 0;
}
