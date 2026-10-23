// SPDX-License-Identifier: GPL-2.0-or-later
// Default for VK_EXT_vertex_input_dynamic_state per driver (spartanport_vertex_input_default.h):
// off on Intel's Windows driver, on everywhere else. No Vulkan device required.
#include <cassert>

#include "video_core/amdgpu/spartanport_vertex_input_default.h"

int main() {
    assert(!spartanport_vertex_input_dynamic_default(VK_DRIVER_ID_INTEL_PROPRIETARY_WINDOWS));
    assert(spartanport_vertex_input_dynamic_default(VK_DRIVER_ID_NVIDIA_PROPRIETARY));
    assert(spartanport_vertex_input_dynamic_default(VK_DRIVER_ID_AMD_PROPRIETARY));
    assert(spartanport_vertex_input_dynamic_default(VK_DRIVER_ID_AMD_OPEN_SOURCE));
    assert(spartanport_vertex_input_dynamic_default(VK_DRIVER_ID_MESA_RADV));
    assert(spartanport_vertex_input_dynamic_default(VK_DRIVER_ID_INTEL_OPEN_SOURCE_MESA));
    assert(spartanport_vertex_input_dynamic_default(VK_DRIVER_ID_MOLTENVK));
    return 0;
}
