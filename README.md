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

My patches for [eFMann's shadPS4 Bloodborne DLSS & FSR build](https://github.com/eFMann/shadPS4-Bloodborne-DLSS-FSR), written on an RX 9070 XT. All three are merged upstream. AMD FSR 4.1 and DLSS 5 NR come with his first [release](https://github.com/eFMann/shadPS4-Bloodborne-DLSS-FSR/releases) after v2.1.1, until then with his [CI builds](https://github.com/eFMann/shadPS4-Bloodborne-DLSS-FSR/actions/workflows/build.yml); install steps are in [his README](https://github.com/eFMann/shadPS4-Bloodborne-DLSS-FSR#install). This repo stays as the record.

## ▓▒░ A M D &nbsp; F S R &nbsp; 4 . 1 ░▒▓

AMD's own FSR 4.1.2 from the FSR SDK 2.4 as its own F1 upscaler, *FSR 4.1.2 (AMD)*, on AMD's signed DLLs. Merged 2026-11-15 ([#51](https://github.com/eFMann/shadPS4-Bloodborne-DLSS-FSR/pull/51)).

## ▓▒░ D L S S &nbsp; 5 &nbsp; N R ░▒▓

NVIDIA's DLSS 5 neural rendering on Radeon RX 9000 through [DLSSNR-AMD](https://gitlab.com/mochizuki0323/DLSSNR-AMD) (MIT), run on the 1440p scene before AMD FSR 4.1. CC_SMALL_FP8 at 2560x1440 costs ~5.7 ms on an RX 9070 XT. The model comes from your own `nvngx_dlssnr.dll`. Merged 2026-11-15 ([#54](https://github.com/eFMann/shadPS4-Bloodborne-DLSS-FSR/pull/54)); eFMann then gave it F1's DLSS 5 tab, shared with NVIDIA RTX cards.

The add-on carries a patch that lets DLSSNR-AMD use ACO on shadPS4's device (about 2 ms faster), reported upstream as [DLSSNR-AMD #5](https://gitlab.com/mochizuki0323/DLSSNR-AMD/-/work_items/5).

## ▓▒░ S D R &nbsp; T O &nbsp; H D R ░▒▓

Special K style SDR to HDR for any GPU, written because the RenoDX add-on does nothing on Radeon. Merged upstream with authorship kept ([#28](https://github.com/IFreemz/shadPS4-Bloodborne-DLSS-FSR/issues/28)) and in every 2.x release: F1 → *SDR to HDR*. The original build for 1.9.0-pre.3 is [v1.9.0-pre.3-retrohdr.1](../../releases/tag/v1.9.0-pre.3-retrohdr.1).

## ▓▒░ G R E E T Z ░▒▓

eFMann for carrying the build * IFreemz for starting it * mochizuki0323 for DLSSNR-AMD * AMD for FSR 4.1 on Vulkan * Cainhurst27 for the VRAM leak hunt * Kaldaien and the Special K crew for the curve * shadPS4 crew * every hunter still waiting for a PC port

## ▓▒░ L I C E N S E ░▒▓

GPL-2.0-or-later, see [LICENSE](LICENSE); per-file details in [eFMann's README](https://github.com/eFMann/shadPS4-Bloodborne-DLSS-FSR#license). Written by Robert Bopko ([zeroznet](https://github.com/zeroznet)) with Boba Bott (Claude). Not affiliated with Sony, FromSoftware, NVIDIA, AMD or the shadPS4 team; no game files included.

<p align="center"><sub><i>fear the old blood. not the milliseconds.</i></sub></p>
