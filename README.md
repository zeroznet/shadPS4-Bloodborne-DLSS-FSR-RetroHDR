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

       -=[ shadPS4 Bloodborne * SDR to HDR * any GPU ]=-
```

<p align="center">
  <a href="https://github.com/zeroznet/shadPS4-Bloodborne-DLSS-FSR-RetroHDR/releases"><img src="https://img.shields.io/github/v/release/zeroznet/shadPS4-Bloodborne-DLSS-FSR-RetroHDR?include_prereleases&style=flat-square&labelColor=000000&color=ff5fff" alt="Release"></a>
  <a href="https://github.com/IFreemz/shadPS4-Bloodborne-DLSS-FSR/releases/tag/v2.0.1"><img src="https://img.shields.io/badge/upstream-IFreemz%202.0.1-00ffff?style=flat-square&labelColor=000000" alt="Merged into IFreemz 2.0.1"></a>
  <a href="LICENSE"><img src="https://img.shields.io/github/license/zeroznet/shadPS4-Bloodborne-DLSS-FSR-RetroHDR?style=flat-square&labelColor=000000&color=875fff" alt="License"></a>
  <img src="https://img.shields.io/badge/status-archived-ffaf00?style=flat-square&labelColor=000000" alt="Archived">
</p>

```text
 ░▒▓█ S P E C S █▓▒░

 TARGET ........ Bloodborne CUSA03173 on IFreemz's shadPS4 DLSS/FSR build
 CURVE ......... Special K Perceptual Boost v1, Color Boost 0.333
 OUTPUT ........ scRGB (FP16) here, HDR10 or scRGB upstream
 GPU ........... any; built because RenoDX skips AMD
 MEASURED ...... 799 nits on the HDR meter for an 800 nit target
 LICENSE ....... GPL-2.0-or-later
```

Special K style SDR to HDR for Bloodborne on shadPS4. Written for an RX 9070 XT, where the RenoDX add-on does nothing, then picked up by IFreemz as is.

## ▓▒░ S T A T U S ░▒▓

Done and archived. IFreemz merged every commit with authorship kept ([#28](https://github.com/IFreemz/shadPS4-Bloodborne-DLSS-FSR/issues/28)) and ships it in [2.0.1](https://github.com/IFreemz/shadPS4-Bloodborne-DLSS-FSR/releases/tag/v2.0.1) and [2.1.0-pre.3](https://github.com/IFreemz/shadPS4-Bloodborne-DLSS-FSR/releases/tag/v2.1.0-pre.3). He added HDR10 output on top, so DLSS frame generation works too. **Use his builds.** This repo keeps the original release on 1.9.0-pre.3.

## ▓▒░ T H E &nbsp; T R I C K ░▒▓

- The present pass takes the stock SDR code value, decodes it with gamma 2.2 and runs Special K's Perceptual Boost (PQ gain 1.273), then scales by the brightness slider. No extra pass.
- The swapchain switches to scRGB at startup when Windows HDR is on for the monitor that holds the window and RenoDX is not loaded. Frame images, FSR frame generation and the HDR meter follow that format.
- Peak comes from DXGI `MaxLuminance`, SDR white from Windows DisplayConfig. The brightness slider stops where SDR white hits the peak.
- The F1 overlay gets its own scRGB shader so it sits at desktop SDR brightness. The game frame pipeline is untouched.
- Unticking the checkbox shows the stock SDR image at the Windows SDR white level, live, for A/B.

## ▓▒░ F 1 &nbsp; C O N T R O L S ░▒▓

| Control | What it does |
|---|---|
| SDR to HDR | First time: tick and restart the game. After that it switches live. |
| HDR brightness | Special K's Brightness Scale. The line below shows where white and 18 % gray land in nits. |
| HDR peak | `auto` takes the Windows value; set your own if the display reports none or a wrong one. |

Settings live in `dlss.ini` (`retro_hdr`, `retro_hdr_brightness`, `retro_hdr_peak`). Start the emulator with `SHADPS4_HDR_METER=1` for a nits readout under the HP bar (FP16 output only).

## ▓▒░ I N S T A L L &nbsp; ( v 1 ) ░▒▓

Only if you want the original build on IFreemz 1.9.0-pre.3:

1. Copy your IFreemz [1.9.0-pre.3](https://github.com/IFreemz/shadPS4-Bloodborne-DLSS-FSR/releases/tag/v1.9.0-pre.3) folder, FSR 4 add-on included.
2. Grab [v1.9.0-pre.3-retrohdr.1](https://github.com/zeroznet/shadPS4-Bloodborne-DLSS-FSR-RetroHDR/releases/tag/v1.9.0-pre.3-retrohdr.1), check it (`sha256sum -c SHA256SUMS`, or `Get-FileHash` in PowerShell) and drop its `shadPS4.exe` into the copy.
3. Add the folder in QtLauncher (Version Manager, Add Custom) and delete `%APPDATA%\shadPS4\cache\CUSA03173` before the first run.

Smoke results: [docs/smoke-2026-10-15.md](docs/smoke-2026-10-15.md). Design and plan: [docs/superpowers/](docs/superpowers/).

## ▓▒░ G R E E T Z ░▒▓

IFreemz for the build and the merge * eFMann for the threaded renderer * Cainhurst27 for the VRAM leak hunt * Kaldaien and the Special K crew for the curve * shadPS4 crew * every hunter still waiting for a PC port

## ▓▒░ L I C E N S E ░▒▓

GPL-2.0-or-later, like the code it builds on, see [LICENSE](LICENSE). Written by Robert Bopko ([zeroznet](https://github.com/zeroznet)) with Boba Bott (Claude).

<p align="center"><sub><i>fear the old blood. not the nits.</i></sub></p>
