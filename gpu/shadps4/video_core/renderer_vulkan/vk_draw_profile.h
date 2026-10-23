// SPDX-License-Identifier: GPL-2.0-or-later
// spartanport: where the CPU time of a draw goes, per section of Rasterizer::Draw (stage A: pipeline
// selection) and Rasterizer::DrawRecord (stage B: binding and recording). Collected only with
// SPARTAN_FRAME_STATS=1 and printed every 5 s as microseconds per draw.
#pragma once

#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <x86intrin.h>

namespace DrawProfile {

enum Section : unsigned {
    // Stage A (Rasterizer::Draw)
    Drain,
    SelectPipeline,
    Post,
    // Stage B (Rasterizer::DrawRecord)
    FrameStart,
    RenderState,
    BindResources,
    BeginRendering,
    VertexIndex,
    Motion,
    DynamicState,
    Record,
    Count,
};

inline constexpr const char* names[Count] = {
    "drain+filter", "select pipeline", "post",
    "frame start",  "render state",    "bind resources", "begin rendering",
    "vertex/index", "motion",          "dynamic state",  "record+kick",
};

inline bool Enabled() {
    static const bool enabled = std::getenv("SPARTAN_FRAME_STATS") != nullptr;
    return enabled;
}

inline std::atomic<std::uint64_t> cycles[Count];
inline std::atomic<std::uint64_t> draws_a{0}, draws_b{0};

/// Attributes the cycles since the previous Mark (or the construction) to a section.
struct Lap {
    bool on = Enabled();
    std::uint64_t last = on ? __rdtsc() : 0;
    void Mark(Section section) {
        if (!on) {
            return;
        }
        const std::uint64_t now = __rdtsc();
        cycles[section].fetch_add(now - last, std::memory_order_relaxed);
        last = now;
    }
};

/// Called once per recorded draw; prints and resets the window every 5 s.
inline void MaybePrint() {
    if (!Enabled()) {
        return;
    }
    const auto b = draws_b.fetch_add(1, std::memory_order_relaxed) + 1;
    if ((b & 1023) != 0) {
        return;
    }
    static auto window = std::chrono::steady_clock::now();
    static std::uint64_t window_tsc = __rdtsc();
    const auto now = std::chrono::steady_clock::now();
    const double seconds = std::chrono::duration<double>(now - window).count();
    if (seconds < 5.0) {
        return;
    }
    const std::uint64_t tsc = __rdtsc();
    const double hz = double(tsc - window_tsc) / seconds;
    const double a = double(draws_a.exchange(0, std::memory_order_relaxed));
    const double bb = double(draws_b.exchange(0, std::memory_order_relaxed));
    std::printf("Draw cost (us/draw; stage A %.0f draws, stage B %.0f draws in %.1f s):", a, bb,
                seconds);
    double total_a = 0, total_b = 0;
    for (unsigned i = 0; i < Count; ++i) {
        const double c = double(cycles[i].exchange(0, std::memory_order_relaxed));
        const double n = i < FrameStart ? a : bb;
        const double us = n > 0 ? c / hz * 1e6 / n : 0.0;
        (i < FrameStart ? total_a : total_b) += us;
        if (us >= 0.05) {
            std::printf(" %s=%.1f", names[i], us);
        }
        if (i == Post || i == Count - 1) {
            std::printf(" [stage %c total %.1f]%s", i == Post ? 'A' : 'B',
                        i == Post ? total_a : total_b, i == Post ? " |" : "");
        }
    }
    std::printf("\n");
    window = now;
    window_tsc = tsc;
}

} // namespace DrawProfile
