// SPDX-FileCopyrightText: Copyright 2026 Robert Bopko
// SPDX-License-Identifier: GPL-2.0-or-later

#include <cstdlib>
#include <filesystem>
#include <mutex>
#include <string_view>

#include "common/logging/log.h"
#include "video_core/renderer_vulkan/vk_bb_library.h"
#include "video_core/renderer_vulkan/vk_bb_neural_rendering.h"
#include "video_core/renderer_vulkan/vk_scheduler.h"

// The add-on is built by another compiler: these are the test that both read one layout.
static_assert(sizeof(ShadNrImage) == 32);
static_assert(sizeof(ShadNrFrame) == 128);
static_assert(sizeof(ShadNrSettings) == 12);
static_assert(sizeof(ShadNrStatus) == 276);

namespace Vulkan {

namespace {
std::filesystem::path AddonDirectory() {
    return BbLibrary::ExecutableDirectory() / "dlssnr-amd";
}

// The network's background builds submit to the graphics queue: held like a present, which takes
// present_mutex (Keep rendering while presenting, separate present queue) and submit_mutex.
void LockQueue(void*) {
    std::lock(Scheduler::present_mutex, Scheduler::submit_mutex);
}

void UnlockQueue(void*) {
    Scheduler::submit_mutex.unlock();
    Scheduler::present_mutex.unlock();
}
} // namespace

struct BbNeuralRendering::Api {
    decltype(&shadps4_nr_abi_version) abi_version;
    decltype(&shadps4_nr_create_device) create_device;
    decltype(&shadps4_nr_init) init;
    decltype(&shadps4_nr_evaluate) evaluate;
    decltype(&shadps4_nr_status) status;
    decltype(&shadps4_nr_shutdown) shutdown;
};

bool BbNeuralRendering::Present() {
#ifdef _WIN32
    static const bool present = [] {
        const char* setting = std::getenv("SHADPS4_NR");
        if (setting && std::string_view{setting} == "0")
            return false;
        return std::filesystem::is_regular_file(AddonDirectory() / "shadps4_nr.dll") &&
               std::filesystem::is_regular_file(AddonDirectory() / "dlssnr.bin");
    }();
    return present;
#else
    return false; // DLSSNR-AMD's Windows runtime is the one built here
#endif
}

bool BbNeuralRendering::SmallModelPresent() {
    static const bool present =
        std::filesystem::is_regular_file(AddonDirectory() / "dlssnr-small.bin");
    return present;
}

std::unique_ptr<BbNeuralRendering> BbNeuralRendering::Load() {
    if (!Present())
        return {};
    auto nr = std::unique_ptr<BbNeuralRendering>{new BbNeuralRendering};
    nr->module = BbLibrary::Load(AddonDirectory() / "shadps4_nr.dll");
    const auto find = [&](const char* name) { return BbLibrary::Find(nr->module, name); };
    nr->api = std::make_unique<Api>(
        Api{reinterpret_cast<decltype(Api::abi_version)>(find("shadps4_nr_abi_version")),
            reinterpret_cast<decltype(Api::create_device)>(find("shadps4_nr_create_device")),
            reinterpret_cast<decltype(Api::init)>(find("shadps4_nr_init")),
            reinterpret_cast<decltype(Api::evaluate)>(find("shadps4_nr_evaluate")),
            reinterpret_cast<decltype(Api::status)>(find("shadps4_nr_status")),
            reinterpret_cast<decltype(Api::shutdown)>(find("shadps4_nr_shutdown"))});
    const auto& a = *nr->api;
    if (!a.abi_version || !a.create_device || !a.init || !a.evaluate || !a.status || !a.shutdown ||
        a.abi_version() != SHADPS4_NR_ABI_VERSION) {
        LOG_WARNING(Render_Vulkan, "[NR] dlssnr-amd/shadps4_nr.dll is missing or from another "
                                   "version; neural rendering off");
        nr->api.reset();
        return {};
    }
    LOG_INFO(Render_Vulkan, "[NR] dlssnr-amd/shadps4_nr.dll loaded");
    return nr;
}

BbNeuralRendering::~BbNeuralRendering() {
    if (api)
        api->shutdown();
    // Once the device was made through it, the model's detached threads may still run its code:
    // the DLL stays loaded until the process ends.
    if (!device_made)
        BbLibrary::Free(module);
}

vk::Result BbNeuralRendering::CreateDevice(vk::PhysicalDevice physical,
                                           const vk::DeviceCreateInfo& info, vk::Device& out) {
    VkDevice device{};
    u32 added{};
    const auto result = static_cast<vk::Result>(api->create_device(
        VULKAN_HPP_DEFAULT_DISPATCHER.vkCreateDevice, static_cast<VkPhysicalDevice>(physical),
        &static_cast<const VkDeviceCreateInfo&>(info), nullptr, &device, &added));
    device_made = true;
    out = vk::Device{device};
    LOG_INFO(Render_Vulkan, "[NR] Device created {} the network's features",
             added ? "with" : "without");
    return result;
}

bool BbNeuralRendering::Init(vk::Instance instance, vk::PhysicalDevice physical, vk::Device device,
                             u32 queue_family) {
    const auto result =
        api->init(static_cast<VkInstance>(instance), static_cast<VkPhysicalDevice>(physical),
                  static_cast<VkDevice>(device), queue_family, LockQueue, UnlockQueue, nullptr);
    const bool ready = result == 0;
    if (ready)
        LOG_INFO(Render_Vulkan, "[NR] Ready on queue family {}", queue_family);
    else
        LOG_WARNING(Render_Vulkan, "[NR] The network's session could not start (see "
                                   "dlssnr-amd.log); neural rendering off");
    return ready;
}

bool BbNeuralRendering::Evaluate(vk::CommandBuffer command, const ShadNrFrame& frame,
                                 const ShadNrSettings& settings) {
    return api->evaluate(static_cast<VkCommandBuffer>(command), &frame, &settings) == 0;
}

ShadNrStatus BbNeuralRendering::Status() const {
    ShadNrStatus status{};
    api->status(&status);
    return status;
}

} // namespace Vulkan
