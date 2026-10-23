// SPDX-License-Identifier: GPL-2.0-or-later
// spartanport: the Vulkan drivers where VK_EXT_vertex_input_dynamic_state stays unused unless
// SPARTAN_VERTEX_INPUT_DYNAMIC=1 says otherwise. vk_instance.cpp decides with it (the static
// vertex input path of the pipeline is used instead).
//
// Intel's Windows driver: with the extension on, the HD 630 draws every 3D object black or blue;
// without it the scene is correct (#35, #288). NVIDIA, AMD, RADV, Mesa ANV and MoltenVK keep it.
#pragma once

#include <vulkan/vulkan_core.h>

static inline int spartanport_vertex_input_dynamic_default(VkDriverId driver) {
    return driver != VK_DRIVER_ID_INTEL_PROPRIETARY_WINDOWS;
}
