<!--
SPDX-FileCopyrightText: 2026 Robert Bopko
SPDX-License-Identifier: GPL-2.0-or-later
-->
<!-- written by Robert Bopko (github.com/zeroznet) with Boba Bott (Claude Opus 5.5) -->

<h1 align="center">shadPS4 Bloodborne DLSS/FSR · RetroHDR</h1>

<p align="center">
  <strong>Bloodborne in HDR on any HDR display, AMD included: Special K style SDR to HDR built into IFreemz's shadPS4 build.</strong>
</p>

<p align="center">
  <a href="https://github.com/zeroznet/shadPS4-Bloodborne-DLSS-FSR-RetroHDR/releases"><img src="https://img.shields.io/github/v/release/zeroznet/shadPS4-Bloodborne-DLSS-FSR-RetroHDR?style=flat&color=blue&include_prereleases" alt="Latest Release"></a>
  <a href="https://github.com/zeroznet/shadPS4-Bloodborne-DLSS-FSR-RetroHDR/commits/main"><img src="https://img.shields.io/github/last-commit/zeroznet/shadPS4-Bloodborne-DLSS-FSR-RetroHDR?style=flat" alt="Last Commit"></a>
  <a href="LICENSE"><img src="https://img.shields.io/github/license/zeroznet/shadPS4-Bloodborne-DLSS-FSR-RetroHDR?style=flat" alt="License"></a>
  <img src="https://img.shields.io/badge/base-IFreemz%201.9.0--pre.3-8a2be2?style=flat" alt="Base">
  <img src="https://img.shields.io/badge/output-scRGB-3fb950?style=flat" alt="scRGB">
</p>

<p align="center">
  <a href="#the-problem">Problem</a> ·
  <a href="#how-it-works">How it works</a> ·
  <a href="#install">Install</a> ·
  <a href="#usage">Usage</a> ·
  <a href="#building">Building</a> ·
  <a href="#credits">Credits</a>
</p>

---

A fork of [IFreemz/shadPS4-Bloodborne-DLSS-FSR](https://github.com/IFreemz/shadPS4-Bloodborne-DLSS-FSR) `v1.9.0-pre.3` (threaded renderer, DLSS / FSR 4 upscaling, frame generation) that adds one thing: an SDR to HDR mode in the F1 panel. It expands the finished SDR frame with Special K's Perceptual Boost and shows it through an scRGB swapchain. Everything else is IFreemz's build, unchanged; see the [upstream README](https://github.com/IFreemz/shadPS4-Bloodborne-DLSS-FSR/blob/v1.9.0-pre.3/README.md) for upscaling, patches and frame generation.

## The problem

Bloodborne is an SDR game. The HDR mod for this build, RenoDX, swaps the game's shaders by matching the hash of the SPIR-V the emulator generates, and shadPS4 generates different SPIR-V on AMD than on NVIDIA, so the mod does not engage on AMD cards. Special K's HDR retrofit needs NVIDIA's DXGI interop for Vulkan games, and Windows AutoHDR and RenoDX's generic add-on cover DirectX only. On AMD there was no way to play it in HDR.

## How it works

```
 upscaler output ─► present pass ─────────────► frame (FP16) ─► game window ─► scRGB swapchain ─► Windows HDR
                    stock SDR code value                         F1 overlay at SDR desktop brightness
                    → gamma 2.2 decode
                    → Special K Perceptual Boost (PQ gain 1.273, Color Boost 0.333)
                    → × brightness
```

- Runs inside the existing present pass: no extra pass, no extra copy.
- The swapchain switches to scRGB (`R16G16B16A16_SFLOAT`, extended sRGB linear) at startup when the setting is on, Windows HDR is on for the display that holds the window, and the driver offers the format.
- Peak brightness comes from Windows (`MaxLuminance` of that display); the brightness slider stops where SDR white reaches it, so SDR white never exceeds the peak.
- With the checkbox off, the same session shows the stock SDR image at the Windows SDR white level: an instant A/B, no restart.

## Install

1. Have IFreemz's [1.9.0-pre.3 release](https://github.com/IFreemz/shadPS4-Bloodborne-DLSS-FSR/releases/tag/v1.9.0-pre.3) set up (plus the FSR 4 add-on if you use it). Copy that folder.
2. Download the [latest RetroHDR release](https://github.com/zeroznet/shadPS4-Bloodborne-DLSS-FSR-RetroHDR/releases), check it (`sha256sum -c SHA256SUMS`, or `Get-FileHash` on Windows), and put its `shadPS4.exe` into the copied folder.
3. Add the folder in your launcher (QtLauncher: Version Manager → Add Custom).
4. Delete `%APPDATA%\shadPS4\cache\CUSA03173` before the first run: a new exe is a new emulator version, and a mixed shader cache can hang startup.

## Usage

Turn on **Use HDR** in Windows display settings. In game press **F1**, section **SDR to HDR**:

| Control | What it does |
|---|---|
| SDR to HDR | First time: tick it and restart the game. After that it switches between HDR and the stock SDR look live. |
| HDR brightness | Special K's Brightness Scale. The line below shows where SDR white and 18 % gray land, in nits. |
| HDR peak | `auto` uses the value Windows reports for the display; set your own if the display reports none or a wrong one. |

Settings are saved in `dlss.ini` (`retro_hdr`, `retro_hdr_brightness`, `retro_hdr_peak`). A change to Windows HDR takes effect after a game restart. White HUD elements are expanded like the rest of the image, as with Special K.

To measure the output, start the emulator with `SHADPS4_HDR_METER=1`: a readout under the frame counter shows peak and median nits.

## Building

GitHub Actions builds every pull request and every push to `main` on `windows-2025-vs2026` (clang-cl + Ninja); the exe is attached to the run as `shadPS4-exe-only-win64-<hash>`. Tags `v1.9.0-pre.3-retrohdr.N` publish a release. Local Windows builds follow the upstream [Windows build guide](https://github.com/IFreemz/shadPS4-Bloodborne-DLSS-FSR/blob/v1.9.0-pre.3/documents/building-windows.md).

## Credits

- [IFreemz](https://github.com/IFreemz/shadPS4-Bloodborne-DLSS-FSR) and contributors for the Bloodborne build this fork sits on.
- [shadPS4](https://github.com/shadps4-emu/shadPS4), the emulator.
- [Special K](https://github.com/SpecialKO/SpecialK) by Kaldaien: the Perceptual Boost curve and its constants come from its HDR shader and widget.

GPL-2.0-or-later, like the code it builds on.
