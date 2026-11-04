# Native DLSS 5 Neural Rendering (AMD) before FSR 4.1.2: design

Status: design, not started. Branch `feat/native-nr` (from `feat/amd-fsr-4.1`, PR eFMann #51 still open).
This file is for planning only; drop it from the branch before opening the PR to eFMann.

## Intent

Robert plays Bloodborne on shadPS4 (this build) on an RX 9070 XT, 1440p render to 4K with AMD FSR 4.1.2
(`dlss.ini` `upscaler=amdfsr`). On 2026-11-04 he tried DLSS 5 Neural Rendering (NR) on AMD through
mochizuki0323's DLSSNR-AMD 0.0.8.2 `optiscaler` route and liked the look ("je to pekne urcite").
That route works only through workarounds, so the goal is the same result built into this emulator:
NR runs as its own pass on the render-resolution frame, right before our FSR 4.1.2, driven by our
F1 panel. First for Robert to play and test, then as a PR to eFMann.

## What exists now (the working reference setup)

Installed in Robert's play build `Downloads\bin\shadPS4-Bloodborne-DLSS-FSR-2.1.0-pre.5-fsr412-win64\`
(manifest `dlssnr-amd-install.txt`; package and the NVIDIA DLL zip in `Downloads\mods\NR\`):

- OptiScaler 10.0.0-dev as `dxgi.dll`, which intercepts our `amd_fsr\amd_fidelityfx_loader_vk.dll` by name,
  converts our FFX Vulkan calls to its NGX path and upscales with its own FSR 4.1.2 (`VulkanUpscaler=ffx`;
  `auto` would be FSR 2.2.1 on AMD).
- Plugin `OptiScaler\plugins\dlssnr-amd-loadlate.asi` + `LateAsiPluginsDelay=8`. Loaded at start it finds no
  `vulkan-1.dll` (shadPS4 loads it via `vk::detail::DynamicLoader` after its static `dxgi.dll` import), takes the
  game for D3D and never rechecks (`nr_asi_main.cpp` `hook_device_creation`, `vk_route_ready`). Loaded late it
  uses its "device creation not seen" fallback (the device from NGX's Vulkan init). Side effect: NR Control (End)
  never opens, because the swap chain exists before the plugin.
- `dlssnr-amd.ini`: `NRHookPoint = 1` (before the upscaler), `NRResolutionScale = 0.67`, RenoDX style, network
  `CC_SILVER_AARDWOLD`.
- Measured: first network build 83 s (frames pass through meanwhile); at scale 1.00 13.7 ms/frame, about 45 fps,
  GPU 100 %; at 0.67 6.3 to 6.8 ms, 60 fps, GPU 90 %. Upscaler 1.6 ms. Log: `dlssnr-amd.log` in the build folder.
- Model: only the genuine NVIDIA-signed `nvngx_dlssnr.dll` 310.8.0 (`CC_SILVER_AARDWOLD`). The faster
  `CC_SMALL_FP8` (310.9.16) needs a genuine 310.9.16 DLL; the copies found (RankFTW/rhi-repo, Hugging Face
  Bandukids) are patched and refused by the extractor's hash check. `CC_SILVER_AARDWOLD_INT4_MIXED` is refused:
  "the game's device has no pipeline binaries".

## Goals

1. NR as a pass in `vk_bb_temporal_dlss.cpp`, on the colour the upscaler is about to consume (render
   resolution, before UI with Decoupled UI), with our depth and motion vectors, recorded into our command
   buffer on our `VkDevice`, then our FSR 4.1.2 as today.
2. No OptiScaler, no late-load plugin, no proxy DLL in the folder.
3. F1 panel controls: NR on/off, network, network resolution scale, intensity. Persisted in `dlss.ini`.
4. Same picture and cost as the reference setup at the same settings (60 fps at scale 0.67 on the RX 9070 XT).
5. Then a PR to eFMann with all CI checks green (clang-format, reuse, windows, linux).

## Non-goals

- Shipping any NVIDIA file or the extracted model. Users bring their own `nvngx_dlssnr.dll` and extract the
  model with mochizuki's tools.
- NR at Present / after the upscaler, NR with the frame generation paths, NR on NVIDIA's own `nvngx_dlssnr.dll`.
- Our own reimplementation of the network: the network is mochizuki's code.
- Linux packaging of the NR runtime (keep it buildable if free, not required).

## Anti-goals (a failure even if it works)

- Any change in behaviour, device features, extensions or performance when the NR files are absent.
- NVIDIA code in shadPS4 itself (same rule as `vk_dlss_ngx.h`: "shadPS4 itself contains no NVIDIA code").
- A first-frame hitch the player cannot avoid: the 80 s network build must not block rendering (frames pass
  through until the network is ready, as the plugin does).
- Stutter regressions with NR on beyond the steady per-frame cost.

## Constraints

- Upstream runtime: mochizuki0323/DLSSNR-AMD on GitLab (MIT; GitHub account suspended; GitHub
  `Overclockers/DLSSNR-AMD` is a stale 0.0.6 mirror). Pin tag `v0.0.8.2` by commit; bumping is a deliberate step.
- Their Windows code builds with mingw (`windows/build/*.sh`, `x86_64-w64-mingw32-g++`). This build is MSVC /
  clang-cl in CI. Keep the runtime a separate DLL with a C ABI, not compiled into shadPS4.
- RX 9000 only (FP8 cooperative matrix through `VK_KHR_cooperative_matrix` + `VK_EXT_shader_float8`).
- Device features the network needs: `shaderFloat16`, `shaderInt8`, `storageBuffer8BitAccess`,
  `vulkanMemoryModel`, `bufferDeviceAddress`, `subgroupSizeControl`, plus the two extensions above.
  shadPS4 already enables all but `vulkanMemoryModel` (`vk_instance.cpp` around line 595); the extensions are
  enabled today only when `amd_fsr\` exists (`vk_instance.cpp` around line 438).
- Repo rules (workspace CLAUDE.md): commit freely, single-line messages, no push without explicit
  per-request approval, no AI attribution in commits or PR text, PR text in Robert's voice signed
  "Cheers, -Robert", posted only on his "pošli". Fix pre-existing clang-format failures in touched files.

## Decided by Robert

- Option 3 over the OptiScaler route; first for him to play and test, then a PR to eFMann.
- He tests everything in game himself. Never launch the game, the emulator or QtLauncher for him.

## My calls (Robert can override)

- **Runtime boundary, data folder, gating:** superseded after reading the pinned tag; see "Decisions changed
  from the spec" in `docs/superpowers/plans/2026-11-04-native-nr.md` (own `dlssnr-amd\shadps4_nr.dll` over the
  model's `create_vk_asi` / `evaluate_vk_asi`, device made through `create_with_network`).
- **Where:** in `vk_bb_temporal_dlss.cpp`, just before the upscaler `Evaluate` (around line 1828, the block that
  picks `ngx`, `fsr4`, `amd_fsr`, `fsr`). Input: `(linear ? linear_color : snapshot)`, `depth_resource`,
  `motion->Resource()`, jitter and reset from `eval`. Write NR's result in place or into a second render-size image
  handed to the upscaler instead. Enable it for `Backend::AmdFsr` first; other backends only after testing.
- **Colour:** the reference setup saw "SDR (legacy codec), frame format 37" with RenoDX processing. Match that first
  (same input format and encoding flags as OptiScaler passed), compare HDR / linear later.
- **Network build:** started at feature creation, frames pass through until ready (core already does this); show
  "NR: building" in F1 instead of a frozen picture.
- **Defaults:** NR off; network `CC_SMALL_FP8` if present else `CC_SILVER_AARDWOLD`; scale 0.67; intensity 1.0.
  New `dlss.ini` keys `nr`, `nr_network`, `nr_scale`, `nr_intensity`, documented in the `dlss.ini` comment and the
  README like `amdfsr`.
- **Failure handling:** a failed load, init or feature creation unloads NR and leaves upscaling untouched (the
  pattern the FSR 4 add-on and `amdfsr` use after commit 5a718613), logged once.
- For the PR, decide with eFMann whether the core DLL ships in the release zip or users take it from mochizuki's
  release.

## Decided by Robert (build)

- **The core DLL is built in CI, not locally** (no `mingw-w64` in WSL). A CI job on this fork checks out the
  pinned mochizuki tag, installs mingw on the runner, runs their build for the NGX core and uploads
  `dlssnr_core.dll` as an artifact next to the shadPS4 exe artifact. Local work in WSL is source reading and
  editing only; every build and test exe comes from CI (Robert approves each push that triggers it).

## Success criteria

1. With the NR files absent: identical log, device extensions and frame time to `feat/amd-fsr-4.1`.
2. With them present, `upscaler=amdfsr`, NR on, scale 0.67: Robert sees the same look as the OptiScaler route,
   60 fps, no new stutter; F1 toggles NR live; F1 shows network, build state, and NR ms.
3. OptiScaler route removed from his play build (`install.ps1 -Route remove`, then check the folder).
4. PR to eFMann: CI green, README + `dlss.ini` docs, no NVIDIA files, licence entries for vendored MIT code (reuse).

## Left to the builder

The C ABI details, image ownership and layouts, how F1 sends settings to the core, the logging shape, and the
build script. Read their `docs/ARCHITECTURE.md`, `windows/src/pe/nr_ngx_core.cpp`, `nr_ngx_abi.hpp` and
`build_optiscaler_nr.sh` at the pinned tag before planning.

## Open items outside the code

- A genuine `nvngx_dlssnr.dll` 310.9.16 (for example from the NBA 2K27 PC update) for `CC_SMALL_FP8`.
- An upstream issue for mochizuki: recheck for `vulkan-1.dll` when it loads later (the plugin already uses
  `LdrRegisterDllNotification` for frame generation libraries). Draft only on Robert's request.
