<!--
SPDX-FileCopyrightText: 2026 Robert Bopko
SPDX-License-Identifier: GPL-2.0-or-later
-->
<!-- written by Robert Bopko (github.com/zeroznet) with Boba Bott (Claude Opus 5.5) -->

**Base:** IFreemz shadPS4-Bloodborne-DLSS-FSR `v1.9.0-pre.3` plus RetroHDR (Special K style SDR to HDR).

**Install:** copy your IFreemz 1.9.0-pre.3 folder, replace `shadPS4.exe` with the one from the zip, add the folder in your launcher, and delete `%APPDATA%\shadPS4\cache\CUSA03173` before the first run. Usage: [README](https://github.com/zeroznet/shadPS4-Bloodborne-DLSS-FSR-RetroHDR).

**Verify:**
```
sha256sum -c SHA256SUMS
Get-FileHash .\shadPS4-RetroHDR-<tag>-win64.zip -Algorithm SHA256  # PowerShell, compare with SHA256SUMS
```
