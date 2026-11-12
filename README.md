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
  <a href="https://github.com/eFMann/shadPS4-Bloodborne-DLSS-FSR/pull/51"><img src="https://img.shields.io/github/pulls/detail/state/eFMann/shadPS4-Bloodborne-DLSS-FSR/51?label=AMD%20FSR%204.1&style=flat-square&labelColor=000000" alt="AMD FSR 4.1 pull request"></a>
  <a href="https://github.com/eFMann/shadPS4-Bloodborne-DLSS-FSR/pull/54"><img src="https://img.shields.io/github/pulls/detail/state/eFMann/shadPS4-Bloodborne-DLSS-FSR/54?label=DLSS%205%20NR&style=flat-square&labelColor=000000" alt="DLSS 5 NR pull request"></a>
  <a href="LICENSE"><img src="https://img.shields.io/github/license/zeroznet/shadPS4-Bloodborne-DLSS-FSR-RetroHDR?style=flat-square&labelColor=000000&color=875fff" alt="License"></a>
</p>

My patches for [eFMann's shadPS4 Bloodborne DLSS & FSR build](https://github.com/eFMann/shadPS4-Bloodborne-DLSS-FSR), written on an RX 9070 XT. They live here until eFMann merges them; the badges show where each pull request stands.

## ▓▒░ A M D &nbsp; F S R &nbsp; 4 . 1 ░▒▓

```text
 SOURCE ........ AMD FSR SDK 2.4, the first release with FSR 4.1 on Vulkan
 BINARIES ...... AMD's signed DLLs, unchanged, in an amd_fsr folder
 DEVICE ........ cooperative matrix + FP8, only with amd_fsr present
 GPU ........... Radeon RX 9000 (RDNA 4), Windows
 TESTED ........ RX 9070 XT, driver 26.10.1, 2560x1440 to 3840x2160
```

Its own upscaler in F1, *FSR 4.1.2 (AMD)*: AMD's own model, not the community v07 port. Delete the folder and everything runs as before.

Branch [`feat/amd-fsr-4.1`](../../tree/feat/amd-fsr-4.1) · [install](../../blob/feat/native-nr/README.md#optional-amd-fsr-41)

## ▓▒░ D L S S &nbsp; 5 &nbsp; N R ░▒▓

```text
 NETWORK ....... NVIDIA's DLSS 5 neural rendering, reimplemented by DLSSNR-AMD (MIT)
 RUNS .......... on the 1440p scene, right before AMD FSR 4.1
 COST .......... CC_SMALL_FP8 at 2560x1440: ~5.8 ms with ACO, ~7.9 ms without
 MODEL ......... made from your own nvngx_dlssnr.dll; nothing from NVIDIA ships here
 GPU ........... Radeon RX 9000 (RDNA 4), Windows
```

[DLSSNR-AMD](https://gitlab.com/mochizuki0323/DLSSNR-AMD) as a small add-on, `shadps4_nr.dll`, that creates the Vulkan device and runs the network each frame; F1 sets the network, its resolution and intensity. The first start builds the network in about 80 seconds while the game runs, later starts take seconds.

DLSSNR-AMD refused ACO on shadPS4's device and fell back to the driver's slower compiler: `VK_EXT_vertex_input_dynamic_state` moves the push constants one user-data slot up. The patch here fixes that and is reported upstream as [DLSSNR-AMD #5](https://gitlab.com/mochizuki0323/DLSSNR-AMD/-/work_items/5).

Branch [`feat/native-nr`](../../tree/feat/native-nr) (on top of FSR 4.1) · [install](../../blob/feat/native-nr/README.md#optional-dlss-5-neural-rendering-on-amd)

## ▓▒░ S D R &nbsp; T O &nbsp; H D R ░▒▓

Special K style SDR to HDR for any GPU, written because the RenoDX add-on does nothing on Radeon. Merged upstream with authorship kept ([#28](https://github.com/IFreemz/shadPS4-Bloodborne-DLSS-FSR/issues/28)) and in every 2.x release: F1 → *SDR to HDR*. The original build for 1.9.0-pre.3 is [v1.9.0-pre.3-retrohdr.1](../../releases/tag/v1.9.0-pre.3-retrohdr.1).

## ▓▒░ G R E E T Z ░▒▓

eFMann for carrying the build * IFreemz for starting it * mochizuki0323 for DLSSNR-AMD * AMD for FSR 4.1 on Vulkan * Cainhurst27 for the VRAM leak hunt * Kaldaien and the Special K crew for the curve * shadPS4 crew * every hunter still waiting for a PC port

## ▓▒░ L I C E N S E ░▒▓

GPL-2.0-or-later, see [LICENSE](LICENSE); per-file details in the [build's README](../../blob/feat/native-nr/README.md#license). Written by Robert Bopko ([zeroznet](https://github.com/zeroznet)) with Boba Bott (Claude). Not affiliated with Sony, FromSoftware, NVIDIA, AMD or the shadPS4 team; no game files included.

<p align="center"><sub><i>fear the old blood. not the milliseconds.</i></sub></p>
