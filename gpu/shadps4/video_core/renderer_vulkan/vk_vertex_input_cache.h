// SPDX-License-Identifier: GPL-2.0-or-later
// spartanport: a pipeline key saved with the static vertex input path holds the vertex buffer
// formats; one saved with VK_EXT_vertex_input_dynamic_state does not. The two must not read each
// other's keys, so the mode is part of the pipeline key version (the first launch after switching
// it, or on Intel's Windows driver where it is off by default, rebuilds the cache once).
#pragma once

#include <atomic>
#include <cstdint>

namespace VertexInputCache {
inline std::atomic<bool> g_dynamic{true};

inline void SetDynamic(bool dynamic) {
    g_dynamic.store(dynamic, std::memory_order_relaxed);
}

inline std::uint32_t PipelineKeyVersion(std::uint32_t base, bool dynamic) {
    return dynamic ? base : base + 0x100u;
}

inline std::uint32_t PipelineKeyVersion(std::uint32_t base) {
    return PipelineKeyVersion(base, g_dynamic.load(std::memory_order_relaxed));
}
} // namespace VertexInputCache
