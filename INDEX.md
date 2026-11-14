# P010 — Announcer: Offline Text-to-Speech on ESP32-S3

| Field | Value |
| :--- | :--- |
| **Document Number** | KBW-P010-IDX-001 |
| **Revision** | R01 |
| **Status** | `[PARKED]` Explored: ver_1 PDM, ver_2 I2S amp, ver_3 natural voice verified |
| **Track** | Utility / Product |
| **Target** | ESP32-S3, ESP-IDF v6.0.2 |
| **Repository** | https://github.com/KezinB/Announcer (public) |
| **Last Updated** | 13 November 2026 |

---

## 1. System Overview

```mermaid
---
config:
  layout: elk
  theme: base
  look: classic
  fontFamily: "'Merriweather Variable', serif"
  themeVariables:
    fontFamily: "'Merriweather Variable', serif"
---
flowchart LR
    UART["UART0 console (typed text)"] --> PREP["text_prep.c (numbers, times, lexicon)"]
    PREP --> TTS["tts.c (chunking, pauses, voices)"]
    TTS --> BE["tts_backend.h"]
    BE --> SAM["tts_sam.c + sam/ (8-bit PCM, 22050 Hz)"]
    SAM --> AUD["audio_out.c (u8 to s16, volume)"]
    AUD --> PDM["I2S PDM TX, GPIO 18"]
    PDM --> RC["100 ohm + 22 nF + 10 uF"]
    RC --> AMP["PAM8403"]
    AMP --> SPK["Speaker"]
```

---

## 2. Document Map

| Path | Purpose |
| :--- | :--- |
| `INDEX.md` | This file: navigation, file map, build reference |
| `project.md` | Product vision, end-state architecture, v2+ roadmap |
| `PROJECT_SPEC.md` | MVP specification (5 features), acceptance criteria |
| `WORK_DONE.md` | Development log and decisions |
| `README.md` | Wiring, pinout, quick start |
| `releases/README.md` | Release ledger |
| `ver_1/docs/ROADMAP.md` | Stage-by-stage bring-up route map (Stages 0-10) |
| `ver_1/docs/SAM_IMPLEMENTATION.md` | Original implementation design with full source listings |
| `ver_1/docs/SPEAKER_TEST.md` | Standalone speaker / amp hardware test procedure |
| `ver_1/docs/AURAVOICE_ENGINE.md` | Parked plan: custom Klatt formant engine to replace SAM |
| `ver_1/docs/notes/` | Earlier drafts and audit prompt (reference only) |
| `ver_1/` | ESP-IDF project, PDM output on GPIO 18 (baseline) |
| `ver_2/` | ESP-IDF project, MAX98357A I2S amp (GPIO 4 DIN, 5 BCLK, 6 LRC) |
| `ver_2/mirror_build.ps1 -Port COM6` | Build and flash ver_2 from `Downloads/Announcer_v2` |
| `ver_3/` | ver_2 + natural-voice clips (Piper, embedded PCM); presets and boot use clips, typed text uses SAM |
| `ver_3/tools/make_clips.py` | Phrase list (`tools/phrases.txt`) -> Piper -> trimmed 22050 Hz PCM in `main/clips/` |

---

## 3. Firmware Map

| File | Role |
| :--- | :--- |
| `ver_1/main/main.c` | Boot: audio init, TTS init, console, boot chime + phrase |
| `ver_1/main/console.c` | UART0 line editor, `/` commands, 5 presets |
| `ver_1/main/text_prep.c` | Number / time expansion, pronunciation replacement table |
| `ver_1/main/tts.c` | Chunking (max 120 chars), punctuation pauses, 5 voice presets |
| `ver_1/main/tts_backend.h` | Engine-agnostic render interface |
| `ver_1/main/tts_sam.c` | SAM backend (only SAM-aware file) |
| `ver_1/main/audio_out.c` | I2S PDM / std I2S output, volume, silence, chime |
| `ver_1/main/sam/` | Third-party SAM C port (licence unclear) |
| `ver_1/main/Kconfig.projbuild` | Output mode, GPIOs, sample rate, volume |

---

## 4. Build Reference

```powershell
cd ver_1
. .\setup_env.ps1
idf.py set-target esp32s3     # first time only
idf.py build
idf.py -p COM6 flash monitor
```

| Script | Use |
| :--- | :--- |
| `ver_1/build_and_flash.ps1 -Port COM6` | Build, flash, monitor (PowerShell) |
| `ver_1/build_and_flash.bat COM6` | Same (cmd) |
| `ver_1/setup_env.ps1` | Activate ESP-IDF v6.0.2 environment |
| `ver_1/mirror_build.ps1 -Port COM6` | `[RECOMMENDED]` Mirror to `Downloads/Announcer_v1`, build and flash there (Drive path too long for ESP-IDF) |
