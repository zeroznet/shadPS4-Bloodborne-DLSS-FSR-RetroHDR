<!--
SPDX-FileCopyrightText: 2026 Robert Bopko
SPDX-License-Identifier: GPL-2.0-or-later
-->
<!-- written by Robert Bopko (github.com/zeroznet) with Boba Bott (Claude Opus 5.5) -->

```text
  ████▄ █████ █████ ████▄ ▄███▄     █   █ ████▄ ████▄
  █   █ █       █   █   █ █   █     █   █ █   █ █   █
  ████▀ ████    █   ████▀ █   █     █████ █   █ ████▀
  █  █  █       █   █  █  █   █     █   █ █   █ █  █
  █   █ █████   █   █   █ ▀███▀     █   █ ████▀ █   █

  -=[ shadPS4 Bloodborne * AMD FSR 4.1 * DLSS 5 NR * SDR to HDR ]=-
```

<p align="center">
  <a href="https://github.com/zeroznet/shadPS4-Bloodborne-DLSS-FSR-RetroHDR/releases"><img src="https://img.shields.io/github/v/release/zeroznet/shadPS4-Bloodborne-DLSS-FSR-RetroHDR?include_prereleases&style=flat-square&labelColor=000000&color=ff5fff" alt="Release"></a>
  <a href="https://github.com/eFMann/shadPS4-Bloodborne-DLSS-FSR/pull/51"><img src="https://img.shields.io/github/pulls/detail/state/eFMann/shadPS4-Bloodborne-DLSS-FSR/51?label=AMD%20FSR%204.1&style=flat-square&labelColor=000000" alt="AMD FSR 4.1 pull request"></a>
  <a href="https://github.com/eFMann/shadPS4-Bloodborne-DLSS-FSR/pull/54"><img src="https://img.shields.io/github/pulls/detail/state/eFMann/shadPS4-Bloodborne-DLSS-FSR/54?label=DLSS%205%20NR&style=flat-square&labelColor=000000" alt="DLSS 5 NR pull request"></a>
  <a href="LICENSE"><img src="https://img.shields.io/github/license/zeroznet/shadPS4-Bloodborne-DLSS-FSR-RetroHDR?style=flat-square&labelColor=000000&color=875fff" alt="License"></a>
</p>

My patches for [eFMann's shadPS4 Bloodborne DLSS & FSR build](https://github.com/eFMann/shadPS4-Bloodborne-DLSS-FSR), written on an RX 9070 XT: AMD's official FSR 4.1 on Vulkan, DLSS 5 neural rendering on Radeon RX 9000, and the SDR to HDR that already ships upstream.

## ▓▒░ S T A T U S ░▒▓

This fork is where the patches live until eFMann merges them. The badges above show where each pull request stands; once one is merged, use his builds.

| Patch | Branch | Upstream |
|---|---|---|
| AMD FSR 4.1.2 | [`feat/amd-fsr-4.1`](../../tree/feat/amd-fsr-4.1) | [#51](https://github.com/eFMann/shadPS4-Bloodborne-DLSS-FSR/pull/51) |
| DLSS 5 NR on AMD | [`feat/native-nr`](../../tree/feat/native-nr) (on top of FSR 4.1) | [#54](https://github.com/eFMann/shadPS4-Bloodborne-DLSS-FSR/pull/54) |
| SDR to HDR (RetroHDR) | merged | in every 2.x release |

## ▓▒░ A M D &nbsp; F S R &nbsp; 4 . 1 ░▒▓

```text
 ░▒▓█ S P E C S █▓▒░

 SOURCE ........ AMD FSR SDK 2.4, the first release with FSR 4.1 on Vulkan
 BINARIES ...... AMD's signed DLLs, unchanged, in an amd_fsr folder
 DEVICE ........ VK_KHR_cooperative_matrix + VK_EXT_shader_float8, only with amd_fsr present
 GPU ........... Radeon RX 9000 (RDNA 4), Windows
 TESTED ........ RX 9070 XT, driver 26.10.1, 2560x1440 to 3840x2160
```

Its own upscaler in F1, *FSR 4.1.2 (AMD)*, next to FSR 3.1 and the community FSR 4 add-on. AMD's own model, not the v07 port. *Automatic* never picks it; delete the folder and everything else runs as before.

## ▓▒░ D L S S &nbsp; 5 &nbsp; N R ░▒▓

```text
 ░▒▓█ S P E C S █▓▒░

 NETWORK ....... NVIDIA's DLSS 5 neural rendering, reimplemented by DLSSNR-AMD (MIT)
 RUNS .......... on the 1440p scene, right before AMD FSR 4.1, on shadPS4's own command buffer
 COMPILER ...... ACO machine code; a patch here makes it accept shadPS4's device
 COST .......... CC_SMALL_FP8 at 2560x1440: ~5.8 ms with ACO, ~7.9 ms with the driver's compiler
 MODEL ......... made from your own nvngx_dlssnr.dll; nothing from NVIDIA ships here
 GPU ........... Radeon RX 9000 (RDNA 4), Windows
```

[DLSSNR-AMD](https://gitlab.com/mochizuki0323/DLSSNR-AMD) wrapped as `shadps4_nr.dll`, a small add-on with a C interface that creates the Vulkan device and runs the network each frame. F1 picks the network, its resolution and intensity. The first start builds the network in about 80 seconds while the game runs; later starts take a few seconds.

The ACO fix is reported upstream as [DLSSNR-AMD #5](https://gitlab.com/mochizuki0323/DLSSNR-AMD/-/work_items/5): shadPS4 turns on `VK_EXT_vertex_input_dynamic_state`, the driver then reserves one more user-data slot, and DLSSNR-AMD refused the layout and fell back to the slower compiler. The patch drops out once a DLSSNR-AMD release has it.

## ▓▒░ S D R &nbsp; T O &nbsp; H D R ░▒▓

Special K style SDR to HDR for any GPU, written because the RenoDX add-on does nothing on Radeon. IFreemz merged it with authorship kept ([#28](https://github.com/IFreemz/shadPS4-Bloodborne-DLSS-FSR/issues/28)) and added HDR10 output; it ships in every 2.x release. F1 has the switch and the brightness and peak sliders.

## ▓▒░ I N S T A L L ░▒▓

FSR 4.1 and NR install like any of eFMann's add-ons: a folder next to `shadPS4.exe`. The steps live in one place, the build's own README:

- [AMD FSR 4.1](../../blob/feat/native-nr/README.md#optional-amd-fsr-41)
- [DLSS 5 neural rendering on AMD](../../blob/feat/native-nr/README.md#optional-dlss-5-neural-rendering-on-amd) (needs AMD FSR 4.1)

Builds of these branches are on [Releases](../../releases), each with `SHA256SUMS.txt`. Delete `%APPDATA%\shadPS4\cache\CUSA03173` when you switch to a different build.

The original RetroHDR release for IFreemz 1.9.0-pre.3 is still there as [v1.9.0-pre.3-retrohdr.1](../../releases/tag/v1.9.0-pre.3-retrohdr.1); smoke results in [docs/smoke-2026-10-15.md](docs/smoke-2026-10-15.md), design and plan in [docs/superpowers/](docs/superpowers/).

## ▓▒░ G R E E T Z ░▒▓

eFMann for carrying the build and the threaded renderer * IFreemz for starting it and merging RetroHDR * mochizuki0323 for DLSSNR-AMD * AMD for putting FSR 4.1 on Vulkan * Cainhurst27 for the VRAM leak hunt * Kaldaien and the Special K crew for the curve * shadPS4 crew * every hunter still waiting for a PC port

## ▓▒░ L I C E N S E ░▒▓

GPL-2.0-or-later, like the code it builds on, see [LICENSE](LICENSE). The NR add-on's interface is MIT; AMD's DLLs come under AMD's license. Details per file are in the build's README. Written by Robert Bopko ([zeroznet](https://github.com/zeroznet)) with Boba Bott (Claude).

NVIDIA and DLSS are trademarks of NVIDIA Corporation, AMD and FidelityFX of Advanced Micro Devices, Inc. Not affiliated with Sony, FromSoftware, NVIDIA, AMD or the shadPS4 team; no game files included.

<p align="center"><sub><i>fear the old blood. not the milliseconds.</i></sub></p>
