<!--
SPDX-FileCopyrightText: 2026 Robert Bopko
SPDX-License-Identifier: GPL-2.0-or-later
-->
<!-- written by Robert Bopko (github.com/zeroznet) with Boba Bott (Claude Opus 5.5) -->

# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project

Robert's public fork of [IFreemz/shadPS4-Bloodborne-DLSS-FSR](https://github.com/IFreemz/shadPS4-Bloodborne-DLSS-FSR): `zeroznet/shadPS4-Bloodborne-DLSS-FSR-RetroHDR` (remote over SSH). Goal: Special K style SDR to HDR retrofit for Bloodborne on AMD (RX 9070 XT, host XSUPERX). The design is canonical in `docs/superpowers/specs/2026-10-14-retro-hdr-design.md`; the implementation plan sits next to it in `docs/superpowers/plans/`.

Progress and next steps live in project memory `retrohdr-status`; Robert's play setup (Windows paths, config, cache rule) in memory `shadps4-bloodborne`. This file holds durable facts only.

License is GPL-2.0-or-later (inherited), not the workspace BSD-2-Clause default. Every new file needs an SPDX header like its siblings, or the `reuse` CI job fails.

## Bootstrap

`git clone` refuses this non-empty directory, so the clone is built in place; the exact steps (fork, Actions enable, in-place init, `main` reset to the base, repo settings, branch protection) are Task 1 of the plan. Fork Actions are disabled by default. Upstream's 2.0.0 `main` stays reachable through the `upstream` remote. Submodules are not needed locally; CI checks them out.

## Base version

Base is tag `v1.9.0-pre.3` (commit `7f4cf4c`; `1f7f685` is the annotated tag object). It has the threaded renderer that fixed Bloodborne traversal stutter; upstream 2.0.0 (`main`) lacks it. Port to 2.1.0-pre when it ships.

Read-only reference clones: `~/.cache/upstream-refs/IFreemz-shadPS4-Bloodborne-DLSS-FSR/` (checked out at upstream `main`; read base files with `git show v1.9.0-pre.3:<path>`, line numbers differ) and `~/.cache/upstream-refs/SpecialKO-SpecialK/` (sparse: HDR shaders and widget, the source of the Perceptual Boost curve).

## Workflow

Same as `zeroznet/wsl-vpnfix`: branch per change (`feat/…`, `docs/…`, `ci/…`), PR, merge commit, branch deleted on merge. Commit messages are single line with a type prefix (`feat:`, `fix:`, `docs:`, `ci:`, `spec:`, `plan:`). Identity `Robert Bopko <zero@tfx.one>`.

## Build and checks

There is no local build loop; CI is the build. `.github/workflows/build.yml` runs on pull requests, pushes to `main` and `workflow_dispatch`; changes only to `**/*.md` or `documents/**` don't trigger it.

- `windows-sdl`: `windows-2025-vs2026`, clang-cl + Ninja, Release with IPO, uploads artifact `shadPS4-exe-only-win64-<shorthash>`. Depends only on `get-info`, so format or REUSE failures don't block the exe.
- `clang-format`: clang-format-19 plus a trailing whitespace grep over `src`. On PRs it checks changed `src/**/*.{cpp,h}` only, elsewhere every file. Style is `.clang-format`.
- `reuse`: `fsfe/reuse-action` license lint over the whole tree.

```sh
gh run watch
gh run download <run-id> -n shadPS4-exe-only-win64-<shorthash>
```

Local static checks (no C++ compiler on this host): `~/.local/bin/clang-format-19` (19.1.7, the CI version; style file is `src/.clang-format`) and `~/.local/bin/glslang` (16.6.0, `--target-env vulkan1.3 -o /dev/null <shader>`).

Unit tests are GTest, off by default (`-DENABLE_TESTS=ON`, then `ctest --test-dir build -R <name>`). Upstream `AGENTS.md` says not to add tests; this feature is host-side presentation and gets verified in game with the HDR meter.

## Architecture: present path

Frame flow: game render, optional DLSS/FSR upscale, `post_process.frag` into a frame image in the swapchain's format, ImGui draws that frame as the game window (stock shader) plus overlays (separate overlay pipeline) onto the swapchain, present. Symbols below are at `v1.9.0-pre.3`.

- `src/video_core/host_shaders/post_process.frag` + `host_passes/pp_pass.h`: final present pass and its push constants `Settings {gamma, hdr, srgb_input}`.
- `vk_presenter.cpp` (`pp_settings.srgb_input =`): true for upscaler output or `A2R10G10B10Srgb`. Robert runs FSR 4, so the frame arrives sRGB-encoded. Frame images and `pp_pass` take their format from the startup surface format; `SetHDR()` does not rebuild them.
- `vk_swapchain.cpp` `FindPresentFormat`: the scRGB branch (`R16G16B16A16_SFLOAT` + `EXTENDED_SRGB_LINEAR`) is gated on `RenoDxLoaded()`. The 9070 XT (driver 26.8.1) exposes that pair. FSR frame generation recreates its context on format change and supports FP16.
- `src/core/libraries/videoout/driver.cpp`: `presenter->SetHDR(req.port->is_hdr)` enables HDR10 only when the game asks. Bloodborne never does, which is why the stock `allowHDR` option does nothing here.
- `src/imgui/dlss_layer.cpp`: F1 panel. It writes `dlss.ini` (`BbDlssSettingsPath()`) through `SettingsFile`, and the renderer polls that file, so panel code needs no renderer access.
- `src/imgui/renderer/imgui_impl_vulkan.cpp`: `pipeline` draws the game frame, `overlay_pipeline` everything else (split so RenoDX's shader replacement only hits the game frame).
- `vk_hdr_meter.cpp`: HDR nits meter, shown with `SHADPS4_HDR_METER=1`; records only when the swapchain is `R16G16B16A16Sfloat`.
- `vk_bb_frame_gen.cpp`: already links DXGI (`adapter->GetDesc1`).
- `src/core/file_sys/bb_hdr_compat.cpp`: RenoDX-only game param rewrite, gated on `RenoDxLoaded()` in `fs.cpp`. Unrelated to this feature; don't widen its gate.

## Upstream code rules

From upstream `AGENTS.md`: match surrounding naming, formatting, helpers and `.clang-format`; comment only when necessary; keep changes minimal, no unrelated reformatting; rebase, never merge `main` into a feature branch; prefer existing abstractions.

## Testing on the host

Robert tests on XSUPERX: copy his IFreemz 1.9.0-pre.3 folder under `Downloads\bin\`, replace `shadPS4.exe` with the CI artifact, add the folder in QtLauncher Version Manager, delete `%APPDATA%\shadPS4\cache\CUSA03173` before first run (a new exe counts as a new emulator version; a mixed cache hangs startup), verify output with the HDR meter. Full working setup (paths, config keys, patches): memory `shadps4-bloodborne`.

Host `vulkaninfo.exe` hangs at exit: run it with a timeout and kill it with `taskkill.exe /IM vulkaninfo.exe /F` (`pkill -f` kills the calling shell).
