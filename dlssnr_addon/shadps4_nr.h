// SPDX-FileCopyrightText: Copyright 2026 Robert Bopko
// SPDX-License-Identifier: MIT

// The C interface of dlssnr-amd/shadps4_nr.dll, the optional DLSS 5 neural rendering add-on:
// mochizuki0323's DLSSNR-AMD network (gitlab.com/mochizuki0323/DLSSNR-AMD, MIT) on shadPS4's own
// Vulkan device, run in place on the colour just before the upscaler. The add-on is built with
// mingw and shadPS4 with MSVC, so everything here is plain C with fixed-size fields; the
// static_asserts below fail one of the two builds if the layouts ever differ.

#pragma once

#include <stdint.h>
#include <vulkan/vulkan_core.h>

#ifdef SHADPS4_NR_BUILD
#define SHADPS4_NR_API __declspec(dllexport)
#else
#define SHADPS4_NR_API
#endif

#define SHADPS4_NR_ABI_VERSION 1u

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ShadNrImage {
    uint64_t image; // VkImage
    uint32_t format, width, height, layout, usage;
} ShadNrImage;

typedef struct ShadNrFrame {
    ShadNrImage color;  // read and written in place, VK_IMAGE_LAYOUT_GENERAL, STORAGE usage
    ShadNrImage depth;  // SHADER_READ_ONLY_OPTIMAL; image 0 = none
    ShadNrImage motion; // SHADER_READ_ONLY_OPTIMAL; image 0 = none
    uint32_t width, height;               // render size: the colour's working extent
    uint32_t output_width, output_height; // the upscaler's output size
    float mv_scale_x, mv_scale_y;
    uint32_t depth_inverted, reset;
} ShadNrFrame;

typedef struct ShadNrSettings {
    int32_t network; // 0 CC_SMALL_FP8, 1 CC_SILVER_AARDWOLD
    float scale;     // the network's resolution, 0.33 to 1 of the frame
    float intensity; // 0 to 2
} ShadNrSettings;

enum { SHADNR_IDLE = 0, SHADNR_BUILDING = 1, SHADNR_RUNNING = 2, SHADNR_FAILED = 3 };

typedef struct ShadNrStatus {
    uint32_t state;
    float network_ms, gpu_ms;
    uint32_t width, height; // the frame of the latest evaluation
    char network[32];
    char message[224];
} ShadNrStatus;

typedef void (*ShadNrQueueLock)(void* context);

SHADPS4_NR_API uint32_t shadps4_nr_abi_version(void);
// vkCreateDevice with the network's extensions and features added; made as asked when that fails
// (*features_added then 0).
SHADPS4_NR_API VkResult shadps4_nr_create_device(PFN_vkCreateDevice create, VkPhysicalDevice physical,
                                                 const VkDeviceCreateInfo* info,
                                                 const VkAllocationCallbacks* allocator,
                                                 VkDevice* out, uint32_t* features_added);
// The network's background builds submit to queue 0 of queue_family between lock and unlock.
// 0 = ready.
SHADPS4_NR_API int32_t shadps4_nr_init(VkInstance instance, VkPhysicalDevice physical,
                                       VkDevice device, uint32_t queue_family, ShadNrQueueLock lock,
                                       ShadNrQueueLock unlock, void* context);
// 0 = ran, or passed the frame through unchanged while the network builds; < 0 = failed.
SHADPS4_NR_API int32_t shadps4_nr_evaluate(VkCommandBuffer cmd, const ShadNrFrame* frame,
                                           const ShadNrSettings* settings);
SHADPS4_NR_API void shadps4_nr_status(ShadNrStatus* out);
// Waits for the device to be idle and drops the session; call before the device is destroyed.
SHADPS4_NR_API void shadps4_nr_shutdown(void);

#ifdef __cplusplus
}
static_assert(sizeof(ShadNrImage) == 32, "ShadNrImage layout");
static_assert(sizeof(ShadNrFrame) == 128, "ShadNrFrame layout");
static_assert(sizeof(ShadNrSettings) == 12, "ShadNrSettings layout");
static_assert(sizeof(ShadNrStatus) == 276, "ShadNrStatus layout");
#endif
