# P010 — Announcer: Development Log

| Field | Value |
| :--- | :--- |
| **Document Number** | KBW-P010-LOG-001 |
| **Revision** | R01 |
| **Last Updated** | 13 November 2026 |

---

## 1. Decision Log

| Date | Decision | Reason |
| :--- | :--- | :--- |
| 2026-11 (pre-import) | SAM as v1 engine behind `tts_backend.h` | Fastest path to speech; engine swappable later |
| 2026-11 (pre-import) | I2S PDM TX on GPIO 18, no DAC | ESP32-S3 has no DAC; PDM needs only an RC filter |
| 2026-11 (pre-import) | 22050 Hz sample rate | Native SAM output rate |
| 2026-11 (pre-import) | `set(COMPONENTS main)` + `-Os` | Work around GCC 15.2 Xtensa issues; faster builds |
| 2026-11 (pre-import) | Custom Klatt engine parked | Needs working baseline and test corpus first |
| 2026-11-13 | v1 uses PDM on GPIO 18; I2S amp (bought) reserved for v2 | Keep v1 matching the existing tested design; I2S path already in `audio_out.c` behind Kconfig |
| 2026-11-13 | Version folder layout: firmware + design docs in `ver_1/`, main docs at project root | Room for `ver_2/` (I2S amp) without disturbing v1 |
| 2026-11-13 | Imported as P010 from `Downloads/ESP32S3_TTS_Test` | Promoted from idea to active build |

---

## 2. Session Log

### 13 November 2026 — Import and Scaffolding

- [x] Firmware copied to `ver_1/` (build output, generated `sdkconfig`, duplicate `files/` and `files.zip` left behind)
- [x] Original docs moved to `ver_1/docs/`; emojis stripped per workspace rule
- [x] Original `README.md` replaced: it described DSP filters, `F` toggle and voices not present in code, and showed a speaker wired directly to GPIO 18
- [x] `.vscode/settings.json` clangd path changed from absolute Downloads path to `${workspaceFolder}/build`
- [x] Code review: BUG-01..03 and CFG-01 logged in `PROJECT_SPEC.md`
- [x] Original build compiled (`esp32s3_speaker_test.bin` present)

### 13 November 2026 — Memory Fixes, PSRAM, First Verified Flash

- [x] BUG-01: `sdkconfig.defaults` set for N16R8 (`SPIRAM`, `SPIRAM_MODE_OCT`, 80 MHz, 16 MB flash)
- [x] BUG-02: SAM tracks `buffersize`; buffer allocated in PSRAM (176 KB), internal 88 KB fallback; `render.c` write guards use real size
- [x] BUG-03: `tts_sam.c` clamps sample count to `GetBufferSize()`, logs truncation
- [x] Build from Google Drive fails (Windows MAX_PATH in mbedtls objects); added `ver_1/mirror_build.ps1` to mirror to `Downloads/Announcer_v1` and build / flash there
- [x] Flashed COM6: chip reports Embedded PSRAM 8MB; boot log `Found 8MB PSRAM`, memory test OK
- [x] Serial test: `Room 12 at 10:05.` normalised to `Room twelve at ten oh five.`; `/i` identical before and after (internal 375191, PSRAM 8383480): no leak
- [x] Audio heard by ear on cap + direct speaker: works, quiet

### 13 November 2026 — ver_2: MAX98357A I2S Amplifier

- [x] `ver_2/` created from `ver_1/`; Kconfig default output switched to standard I2S
- [x] I2S pins moved from GPIO 19/20 (native USB D-/D+) to DIN 4, BCLK 5, LRC 6
- [x] `audio_out.c`: mono sample sent on both slots (`I2S_STD_SLOT_BOTH`) so the MAX98357A (L+R)/2 mix is not 6 dB down
- [x] Built via `ver_2/mirror_build.ps1` (`Downloads/Announcer_v2`), flashed COM6
- [x] Serial verified: `Standard I2S mode configured at 22050 Hz`, speech + text prep OK, heap stable
- [x] Amp wired by Kezin (VIN 5V, DIN 4, BCLK 5, LRC 6)

### 14 November 2026 — ver_2 Rebuild and Scripted Serial Test

| Test | Result |
| :--- | :---: |
| Boot in I2S mode, boot phrase | `[PASS]` |
| Chime, preset 1 | `[PASS]` |
| Text prep: `Room 12 at 10:30 AM, period 3 starts in 5 minutes.` -> `Room twelve at ten thirty A M, period three starts in five minutes.` | `[PASS]` |
| Voice cycle (all 5) | `[PASS]` |
| Volume 50% -> 100% steps | `[PASS]` |
| 200+ character unpunctuated sentence (chunking) | `[PASS]` no crash, no truncation warning |
| Heap before / after full run (internal 375215, PSRAM 8383480) | `[PASS]` identical |
| Unexpected reset | `[FAIL]` 1x `rst:0x1 POWERON` mid-run (~36 s, Classic SAM voice, 70%) |
| Reproduction: 7x preset 4, Classic SAM, 70% and 100% | `[PASS]` no reset |

- [ ] Root cause of single POWERON reset: supply dip or EN line glitch (POWERON = power or EN reset, not a firmware panic). Suspects: loose VIN/GND jumper, USB port sag under amp load. Mitigation: 470-1000 uF across amp VIN/GND, firm wiring, powered USB port
- [ ] Loudness and clarity confirmed by ear
- [x] By ear: all 5 SAM voices sound robotic (expected: 1982 rule-based formant engine, 8-bit / 4-bit output)

### 14 November 2026 — ver_3: Natural-Voice Clip Prototype

| Decision | Reason |
| :--- | :--- |
| Voice features ESP32-S3 only | PSRAM and flash headroom; classic ESP32 projects excluded |
| Piper (offline, PC-side) + `en_US-lessac-medium` voice | Natural voice, 22050 Hz output matches firmware rate, no resampling |
| Clips embedded via `EMBED_FILES`, 6 MB factory partition | Simplest prototype; flash-partition clip pack comes with the component (Phase 2) |
| SAM kept for typed text | A/B comparison and fallback for unknown text |

- [x] Piper + voice downloaded to `Downloads/Announcer_tools/` (outside Drive)
- [x] `ver_3/`: `clips.c/.h`, `audio_play_s16le()`, presets `/1`..`/5` and boot phrase play clips, `partitions.csv`
- [x] `tools/make_clips.py` + `tools/phrases.txt`
- [x] Test build with placeholder clips: `[PASS]`, no warnings
- [x] Clips generated with Piper (user-approved run): 6 clips, 718 KB, 1.6-3.9 s each
- [x] Flashed COM6 (app 940 KB in 6 MB partition); serial test: boot clip, `/1`..`/5` clips, SAM A/B sentence, no reset, heap unchanged
- [x] By ear (Kezin): natural Piper clips sound great; direction confirmed: clips primary, SAM fallback
- [ ] Voice licence: lessac dataset (Blizzard 2013 licence) fine for personal use; check before any commercial use

### 14 November 2026 — Parked

| Item | State |
| :--- | :--- |
| Status | `[PARKED]` by Kezin: exploration complete |
| Best build | `ver_3` (MAX98357A + Piper clips, SAM fallback) |
| Resume step 1 | Choose final Piper voice |
| Resume step 2 | Word-level clip vocabulary (numbers, times) with SAM fallback |
| Resume step 3 | Non-blocking shared `auravoice` component for S3 projects |
| Off-Drive files | `Downloads/Announcer_tools` (Piper + voice), `Downloads/Announcer_v1..v3` (build mirrors) |
