// SPDX-FileCopyrightText: Copyright 2026 Robert Bopko
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <memory>

#include "common/types.h"
#include "video_core/renderer_vulkan/vk_common.h"

#include "../../../dlssnr_addon/shadps4_nr.h"

namespace Vulkan {

// The optional DLSS 5 neural rendering add-on: dlssnr-amd/shadps4_nr.dll next to the executable
// with the model mochizuki0323's DLSSNR-AMD installer extracts (dlssnr-amd/dlssnr.bin). It runs
// the network on shadPS4's device, so the device is created through it; when its files are absent
// nothing about the device changes.
class BbNeuralRendering {
public:
    // Both files present and SHADPS4_NR is not "0"; Windows only.
    static bool Present();
    // dlssnr-amd/dlssnr-small.bin present: CC_SMALL_FP8 can be offered.
    static bool SmallModelPresent();
    // Null when the add-on cannot be loaded or is from another version.
    static std::unique_ptr<BbNeuralRendering> Load();
    ~BbNeuralRendering();

    vk::Result CreateDevice(vk::PhysicalDevice physical, const vk::DeviceCreateInfo& info,
                            vk::Device& out);
    bool Init(vk::Instance instance, vk::PhysicalDevice physical, vk::Device device,
              u32 queue_family);
    // False: the network failed; the caller stops using the add-on.
    bool Evaluate(vk::CommandBuffer command, const ShadNrFrame& frame,
                  const ShadNrSettings& settings);
    ShadNrStatus Status() const;

private:
    BbNeuralRendering() = default;
    struct Api;
    void* module{};
    std::unique_ptr<Api> api;
    bool device_made{};
};

} // namespace Vulkan
