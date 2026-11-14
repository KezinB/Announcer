# ESP32-S3 Speaker Hardware Test Suite (ESP-IDF)

A small, separate ESP-IDF project to test your audio wiring, amplifier and sound quality on the **ESP32-S3** before adding speech. Run this first. If it sounds clean here, any problem later is in software.

---

## 1. Why Test the Speaker First?

The ESP32-S3 has **no built-in DAC** (the old `dacWrite` on GPIO 25/26 existed only on the original ESP32). This project uses the S3's **I2S PDM TX** hardware instead. It turns 16-bit audio into a fast 1-bit pulse stream on **GPIO 18**, and a simple filter turns that back into analog sound.

Testing first confirms:
1. The wiring and amplifier work.
2. The volume level is sensible and nothing clips.
3. Low, mid and voice-range tones come out clean.

---

## 2. Hardware Wiring

```
ESP32-S3
┌──────────┐
│          │
│  GPIO 18 ├──[ 100 Ω ]──┬───────┤|──────────────► Amp input (L or R)
│          │             │      10 µF
│          │          [ 22 nF ]  (+ toward the resistor)
│          │             │
│      GND ├─────────────┴──────────────────────► Amp GND
└──────────┘
```

| Pin | Connection | Function |
|---|---|---|
| **GPIO 18** | 100 Ω resistor, then 10 µF capacitor, then amp input | 1-bit PDM audio out |
| **GND** | Amp GND, 22 nF capacitor | Ground |
| **USB (UART port)** | PC | Power, flashing, monitor |

The speaker connects to the **amplifier's output terminals**, not to the ESP32.

### Important notes

> **Do not connect a speaker directly to GPIO 18.** An ESP32 pin supplies only a few tens of milliamps. An 8 Ω speaker would demand several hundred, giving very quiet sound and stressing the pin. Use an amplifier such as a **PAM8403 module** (a few dollars), a powered speaker, or a PA aux input.

> **Quick bench test without an amp:** high-impedance earphones through a 220 Ω resistor and the 10 µF capacitor will work quietly. Keep the software volume low.

- **100 Ω + 22 nF** is a first-order low-pass (cutoff near 72 kHz). It removes most of the PDM switching noise before the amp. It will not eliminate all RF noise.
- **10 µF capacitor** blocks the DC offset. The positive leg goes toward the resistor / GPIO side.
- The output swings the full 3.3 V. If the amp is loud or distorted at low software volume, add a **10 kΩ potentiometer** at the amp input and turn it down.
- Power the amp from the 5 V / USB pin, not from a GPIO.

---

## 3. Built-in Tests

On boot the board plays a welcome chime so you can check sound immediately. In the monitor (`idf.py monitor`) press a key:

| Key | Test | Purpose |
|:---:|---|---|
| `1` | **Bell chime** (C5, E5, G5, C6) | Clean four-tone chime with natural decay |
| `2` | **Frequency sweep** (200 Hz to 3000 Hz, about 2.5 s) | Checks speaker and amp response, rattles and distortion |
| `3` | **C-major scale** (C4 to C5) | Confirms correct pitch |
| `4` | **Siren warble** (850 Hz / 1250 Hz) | Alarm tone for drills |
| `5` | **Vowel formant test** (A, E, I, O, U) | Approximates vowel sounds; if these are clear, speech will be too |
| `6` | **1000 Hz reference tone** (toggle) | Level setting, scope or meter checks |
| `+` / `-` | Volume up / down by 10% | Software gain |
| `?` | Show menu | |

---

## 4. Build and Flash

### Option A: Scripts (Windows)

```powershell
.\build_and_flash.ps1 -Port COM6
```
or
```cmd
build_and_flash.bat COM6
```
If you leave out the port, the script lists the available COM ports.

### Option B: ESP-IDF commands

```powershell
. .\setup_env.ps1          # activate the environment
idf.py set-target esp32s3  # first time only
idf.py build
idf.py -p COM6 flash monitor
```
Exit the monitor with `Ctrl + ]`. Replace `COM6` with your actual port (check Device Manager).

---

## 5. Settings (menuconfig)

```powershell
idf.py menuconfig
```

```
Speaker Test Configuration
  -> PDM Audio Pin                   (default 18)
  -> Audio Sample Rate (Hz)          (set to 22050 to match the SAM project)
  -> Default Boot Volume (0-100%)    (default 70)
  -> Audio Output Mode
       [*] DAC-free PDM 1-bit TX
       [ ] Standard I2S TX (external MAX98357A / PCM5102 DAC)
```

Testing at **22050 Hz** means you are checking the exact rate the speech project will use.

---

## 6. Troubleshooting

| Symptom | Likely cause |
|---|---|
| Silence | Amp unpowered, wrong amp input, volume 0, or capacitor reversed |
| Quiet and thin | Speaker wired directly to GPIO 18 instead of through an amp |
| Hiss or whine | Missing 22 nF filter capacitor, poor ground, or amp gain too high |
| Distortion on loud tones | Signal too strong for amp input: add a pot or lower software volume |
| Crackle at start or stop | Normal for a DC-coupled amp input; make sure the 10 µF capacitor is fitted |
| Monitor shows nothing | Wrong USB port (use the UART one) or wrong COM port |

When the tests sound clean, move on to the main project (`README.md`, `project.md`).
