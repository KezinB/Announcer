"""Generate natural-voice clips for Announcer ver_3 with Piper.

Reads tools/phrases.txt (name|text), runs Piper for each phrase, checks the
output is 22050 Hz / mono / 16-bit, trims leading and trailing silence, and
writes raw little-endian PCM to main/clips/<name>.pcm for EMBED_FILES.

Usage:
    python make_clips.py --piper <path to piper.exe> --model <voice .onnx>
"""
import argparse
import array
import subprocess
import sys
import tempfile
import wave
from pathlib import Path

HERE = Path(__file__).resolve().parent
PHRASES = HERE / "phrases.txt"
OUT_DIR = HERE.parent / "main" / "clips"
RATE = 22050
SILENCE = 300        # |sample| below this counts as silence
PAD = int(RATE * 0.05)  # keep 50 ms around the speech


def load_phrases():
    items = []
    for line in PHRASES.read_text(encoding="utf-8").splitlines():
        line = line.strip()
        if not line or line.startswith("#"):
            continue
        name, text = line.split("|", 1)
        items.append((name.strip(), text.strip()))
    return items


def synth(piper, model, text, wav_path):
    subprocess.run([str(piper), "--model", str(model), "--output_file", str(wav_path)],
                   input=text.encode("utf-8"), check=True,
                   stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)


def read_and_trim(wav_path):
    with wave.open(str(wav_path), "rb") as w:
        if (w.getframerate(), w.getnchannels(), w.getsampwidth()) != (RATE, 1, 2):
            sys.exit(f"[FAIL] {wav_path.name}: expected {RATE} Hz mono 16-bit, got "
                     f"{w.getframerate()} Hz, {w.getnchannels()} ch, {8 * w.getsampwidth()}-bit")
        samples = array.array("h", w.readframes(w.getnframes()))
    if sys.byteorder != "little":
        samples.byteswap()
    loud = [i for i, s in enumerate(samples) if abs(s) > SILENCE]
    if not loud:
        return samples
    start = max(0, loud[0] - PAD)
    end = min(len(samples), loud[-1] + PAD)
    return samples[start:end]


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--piper", required=True, type=Path)
    ap.add_argument("--model", required=True, type=Path)
    args = ap.parse_args()

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    total = 0
    with tempfile.TemporaryDirectory() as tmp:
        for name, text in load_phrases():
            wav = Path(tmp) / f"{name}.wav"
            synth(args.piper, args.model, text, wav)
            pcm = read_and_trim(wav)
            out = OUT_DIR / f"{name}.pcm"
            out.write_bytes(pcm.tobytes())
            total += out.stat().st_size
            print(f"[DONE] {name:6s} {len(pcm) / RATE:5.2f} s  {out.stat().st_size / 1024:7.1f} KB  {text}")
    print(f"[PASS] {total / 1024:.0f} KB total in {OUT_DIR}")


if __name__ == "__main__":
    main()
