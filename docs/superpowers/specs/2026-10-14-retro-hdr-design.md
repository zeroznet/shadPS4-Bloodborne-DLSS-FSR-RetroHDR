<!--
SPDX-FileCopyrightText: 2026 Robert Bopko
SPDX-License-Identifier: GPL-2.0-or-later
-->
<!-- written by Robert Bopko (github.com/zeroznet) with Boba Bott (Claude Opus 5.5) -->

# RetroHDR: SDR to HDR for Bloodborne on AMD and NVIDIA — Design

**Date:** 2026-10-14
**Status:** Approved
**Author:** Robert Bopko, with Claude Opus 5.5 assistance

Adds a Special K style HDR retrofit to IFreemz's shadPS4 Bloodborne fork: the finished SDR frame is expanded to HDR in the existing present pass with Special K's Perceptual Boost curve and shown through an scRGB swapchain. It works on AMD, where the RenoDX Bloodborne add-on does not. Base is IFreemz `v1.9.0-pre.3` (`7f4cf4c`), the last release with the threaded renderer.

---

## 1. Decisions resolved

| # | Decision | Choice | Rationale |
|---|---|---|---|
| RH-D-1 | HDR source | Inverse tone map of the final SDR frame | RenoDX matches host SPIR-V by hash, and shadPS4 emits vendor-specific SPIR-V (AMD declares `SPV_AMD_shader_explicit_vertex_parameter`, NVIDIA `SPV_KHR_fragment_shader_barycentric`, plus per-vendor lowering in `emit_spirv.cpp` and `vk_pipeline_cache.cpp`), so its Bloodborne add-on cannot match on AMD. The add-on is distributed on Discord only. Special K Vulkan needs NVIDIA DXGI interop; Windows AutoHDR and RenoDX Generic are DirectX only. |
| RH-D-2 | Curve | Special K Perceptual Boost v1, exact constants, Color Boost 0.333 | Proven look Robert knows from SKIF; read from SK source (`resource/shaders/HDR/basic_hdr_shader_ps.hlsl`, `src/widgets/hdr.cpp`), not paraphrased. A two-anchor variant (white on peak, gray on paper white) was evaluated and rejected: it forces the PQ gain to 3.5 to 19 and crushes shadows. |
| RH-D-3 | SDR decode for the boost | Power 2.2 | Windows decodes SDR with the piecewise sRGB curve, which lifts near-black on an OLED; 2.2 is how the game looked on a TV. Fixed, not a setting. |
| RH-D-4 | Brightness | SK Brightness Scale: linear multiplier applied after the boost, default 2.0x (SK preset 0), range 0.30x to `peak / 266.6` | Matches SK: the boost maps SDR white to 266.6 nits at 1.0x, and SK caps the slider so white never exceeds the display peak. No tone-mapping shoulder is needed. |
| RH-D-5 | Display peak | `IDXGIOutput6::GetDesc1().MaxLuminance` of the output showing the window, F1 override, fallback 1000 nits when below 100 | Robert's original requirement: set max nits once, the rest follows. |
| RH-D-6 | Swapchain | scRGB (`R16G16B16A16_SFLOAT` + `EXTENDED_SRGB_LINEAR`) chosen at swapchain creation, the same branch RenoDX uses | Frame images and the post-process pipeline take their format from the startup surface format; the existing live HDR switch does not rebuild them. The RenoDX scRGB configuration already works with the frame images, ImGui, FSR frame generation and the HDR meter. Present on the RX 9070 XT, driver 26.8.1. |
| RH-D-7 | Toggle | F1 checkbox. In an scRGB session it switches the shader mode live (instant A/B, no recreate). Enabling it in an SDR session takes effect after a restart | No swapchain recreate on toggle, so no hitch and no frame generation reinit. IFreemz already labels restart-only options "(restart)". |
| RH-D-8 | Off mode | Stock look: the stock SDR code value, decoded with the piecewise sRGB curve and scaled to the Windows SDR white level | What Windows shows for the unpatched SDR swapchain, so A/B compares against the real baseline. |
| RH-D-9 | ImGui overlay in scRGB | Overlay pixels sRGB-decoded and scaled to the Windows SDR white level | Without it the overlay is written as linear 0..1: dim (80 nits max) and with wrong gamma. The game frame keeps the stock pipeline and passes through unchanged. |
| RH-D-10 | Precedence | RenoDX loaded → RetroHDR unavailable | RenoDX owns the HDR output when present. |
| RH-D-11 | Windows HDR state | Read once at swapchain creation | A mid-session Windows HDR change applies after restart; documented in the README. |
| RH-D-12 | Tests | No unit tests; verification by the F1 HDR meter and a smoke document | Upstream `AGENTS.md` forbids tests in the tree; CI has no test job. |

---

## 2. Pipeline and math

```
upscaler output ─► post_process.frag ─► frame image (FP16) ─► ImGui game window (stock shader) ─► scRGB swapchain
                                                                ImGui overlay (scRGB shader)    ─┘
```

`post_process.frag`, per pixel, when RetroHDR is active:

1. `linear = srgb_input ? degamma(sample) : sample` (existing code path).
2. `stock = clamp(gamma(linear), 0, 1)`: exactly the value the stock SDR path writes to the swapchain, user gamma slider included.
3. Off mode: `out = degamma(stock) * sdr_white_nits / 80`.
4. On mode, Special K Perceptual Boost:
   - `L = pow(stock, 2.2)`; `XYZ = Rec709_to_XYZ(L)`; `Y = max(XYZ.y, 0)`.
   - `B(v) = PQToLinear(LinearToPQ(v, 1.0) * 1.273, 0.1) / 0.5`, where `LinearToPQ(v, m)` is ST 2084 encode of `max(v, 0) / m` and `PQToLinear(c, m)` its inverse times `m` (SK `common_defs.hlsl` definitions).
   - `luma_path = XYZ * B(Y) / Y`; `channel_path = B(XYZ)` per component; `XYZ' = mix(luma_path, channel_path, 0.333)`; pixels with `Y == 0` stay black.
   - `out = XYZ_to_Rec709(XYZ') * brightness`.
5. Alpha passes through as in the stock path (the game window is drawn by ImGui with blending); values are in scRGB units (1.0 = 80 nits).

Reference values (SDR linear input → output nits, peak 800): brightness 2.0x gives 1 % → 1.2, 18 % → 46, 50 % → 192, 100 % → 533; brightness 3.0x (slider maximum) gives 1.8, 70, 288, 800. `B(1) * 80 = 266.6` nits.

Matrices are the SK ones (BT.709 primaries, D65). The CPU mirror used for the F1 readout implements `B` with the same constants.

---

## 3. Changes

| File | Change |
|---|---|
| `src/video_core/renderer_vulkan/vk_retro_hdr.{h,cpp}` (new) | Settings read from `dlss.ini` via `BbDlssSettingsPath()`; display query for the window's `HMONITOR` (DXGI: Windows HDR active when `ColorSpace == DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020`, `MaxLuminance`; `QueryDisplayConfig` + `DisplayConfigGetDeviceInfo(DISPLAYCONFIG_DEVICE_INFO_GET_SDR_WHITE_LEVEL)`: SDR white nits = `SDRWhiteLevel / 1000 * 80`, fallback 203); session state (active, unavailable reason); CPU mirror of `B` for the readout. Windows-only code under `_WIN32`; other platforms report unavailable. |
| `src/video_core/renderer_vulkan/vk_swapchain.cpp` | `FindPresentFormat`: take the scRGB branch when `RenoDxLoaded()` or RetroHDR resolves active; log one line with peak, SDR white and reason. |
| `src/video_core/host_shaders/post_process.frag`, `host_passes/pp_pass.h` | Push constants extended with `retro_mode` (0 none, 1 off, 2 on), `brightness`, `sdr_white` (nits / 80); shader path from section 2. |
| `src/video_core/renderer_vulkan/vk_presenter.cpp` | Fill the new push constants from the polled settings each frame. |
| ImGui renderer (`src/imgui/renderer/imgui_impl_vulkan.cpp`) | Overlay pipeline uses an scRGB fragment shader (sRGB decode × SDR white) when RetroHDR is active. Game-frame pipeline unchanged. |
| `src/imgui/dlss_layer.cpp` | "SDR→HDR" section: checkbox, Brightness slider with readout "white ≈ X nits, 18 % gray ≈ Y nits", Peak line "auto: N nits (Windows)" with override, state text when unavailable or pending restart. |

`dlss.ini` keys: `retro_hdr` (0/1, default 0), `retro_hdr_brightness` (default 2.0), `retro_hdr_peak` (0 = DXGI value).

Out of scope: frame generation changes, HUD separation (a HUD-less image exists only with frame generation), saturation and gamut sliders, Linux and macOS HDR, porting to 2.1.0-pre.

---

## 4. Failure modes

| Condition | Behavior |
|---|---|
| Windows HDR off on the window's display | SDR swapchain, as stock; F1: "Windows HDR is off" |
| Surface lacks the scRGB format | SDR swapchain; F1 and log state it |
| DXGI or DisplayConfig query fails | SDR swapchain (DXGI) or SDR white fallback 203 nits (DisplayConfig); one log line |
| `MaxLuminance < 100` | Peak 1000 nits, override available |
| RenoDX loaded | RetroHDR unavailable, RenoDX path unchanged |
| Setting enabled in an SDR session | F1: "restart to apply" |

---

## 5. Performance

No new pass. The existing fullscreen present pass gains a few `pow` and two PQ conversions per pixel; at 4K on the RX 9070 XT the expected cost is around 0.05 ms (estimate, measured in the smoke run). The FP16 swapchain is 66 MB per frame at 4K. CPU work happens only when settings change, on the existing polling path. Frame time with RetroHDR on must match the baseline within measurement noise.

---

## 6. Repository setup

Mirrors `zeroznet/wsl-vpnfix`:

- Fork `zeroznet/shadPS4-Bloodborne-DLSS-FSR-RetroHDR`; fork `main` reset to `7f4cf4c`. `upstream` remote tracks IFreemz.
- Repo settings: issues on; wiki, projects, discussions off; merge commits only; delete branch on merge. Description and topics set.
- `main` protection: pull request required, `enforce_admins`, conversation resolution, no force push, no deletion. No required status checks: upstream `build.yml` ignores `**/*.md`, so a docs-only PR would wait forever for a check that never runs. A code PR merges only after its CI run is green.
- Branch per change (`feat/…`, `docs/…`, `ci/…`), PR, merge commit. Commit messages single line with a type prefix.
- New files: README in Robert's format (header, badges, navigation, how it works, install, F1 controls, building, credits to IFreemz, shadPS4 and Special K), PR template, `TODO.md`, `CLAUDE.md`, this spec and the plan, smoke documents. Every new file carries an SPDX header for the `reuse` job, plus the attribution line.
- Releases: tag `v1.9.0-pre.3-retrohdr.N`; a release job in `build.yml` packages `shadPS4.exe` and `SHA256SUMS`. Install: copy the exe into a copy of IFreemz's 1.9.0-pre.3 folder, as their own release notes describe.

---

## 7. Verification

1. **Baseline:** CI build of the untouched base. Robert runs it against IFreemz's official 1.9.0-pre.3 exe; frame time and image must match. Fixes the CI build as a known-good reference before any feature change.
2. **CI:** every PR green on the required checks.
3. **Smoke on XSUPERX** (`SHADPS4_HDR_METER=1`), recorded in `docs/smoke-<date>.md`:
   - On mode: meter peak within 5 % of the readout's white value on a white HUD element or the moon.
   - Off mode: visually identical to the unpatched build on the same scene.
   - Live toggle: no hitch in the frame time graph.
   - FSR frame generation on and off: no crash, image correct.
   - Windows HDR off: stock SDR behavior.
   - Frame time: on mode equal to baseline within noise.
   - F1 overlay readable and at desktop SDR brightness.

---

## 8. Execution

Implementation plan via `superpowers:writing-plans`, executed with `superpowers:subagent-driven-development` by Sonnet 5.5 implementer subagents, one task per subagent, reviewed before the next. Subagents commit only; every push, the fork creation and every tag are separately approved by Robert.
