<!--
SPDX-FileCopyrightText: 2026 Robert Bopko
SPDX-License-Identifier: GPL-2.0-or-later
-->
<!-- written by Robert Bopko (github.com/zeroznet) with Boba Bott (Claude Opus 5.5) -->

# TODO — shadPS4-Bloodborne-DLSS-FSR-RetroHDR

Open work only. When an item is done, delete the line. Decisions live in the spec, git log and PRs.

## Now

- [ ] **Ship RetroHDR v1.** Plan: `docs/superpowers/plans/2026-10-14-retro-hdr.md`. Done when the smoke document is merged and `v1.9.0-pre.3-retrohdr.1` is released.

## Later

- [ ] **Port to IFreemz 2.1.0-pre.** When it ships with the threaded renderer: rebase `main` onto the new tag, re-read `FindPresentFormat`, `post_process.frag`, the ImGui overlay pipeline and the F1 panel for drift, rebuild, smoke again.

## Backlog

- [ ] **Offer RetroHDR upstream to IFreemz.** Decide whether to open a PR once v1 has some play time behind it.
