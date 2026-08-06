#!/usr/bin/env python3
"""Extract audio from video files into game-ready ogg/wav."""
import os, subprocess, sys
from pathlib import Path

AUDIO_DIR = Path("/home/sin/Projects/te-halo-inspired/assets/audio/music")
TRANS_DIR = Path("/home/sin/Projects/te-halo-inspired/assets/audio/transitions")
INPUT_DIR = Path("/home/sin/Desktop/music videos")

AUDIO_DIR.mkdir(parents=True, exist_ok=True)
TRANS_DIR.mkdir(parents=True, exist_ok=True)

def extract(src: Path, dst: Path):
    cmd = [
        "ffmpeg", "-y", "-i", str(src),
        "-vn", "-acodec", "libvorbis", "-q:a", "4",
        str(dst)
    ]
    p = subprocess.run(cmd, capture_output=True, text=True)
    if p.returncode == 0:
        print(f"[extract] {src.name} -> {dst.name}")
    else:
        print(f"[extract] FAIL {src.name}: {p.stderr[-200:]}", file=sys.stderr)

extracted = 0
for f in sorted(INPUT_DIR.iterdir()):
    if f.suffix.lower() in {".mp4", ".mkv", ".webm", ".mov", ".avi"}:
        out = AUDIO_DIR / (f.stem + ".ogg")
        extract(f, out)
        extracted += 1
    elif f.suffix.lower() in {".mp3", ".wav", ".flac", ".m4a", ".ogg"}:
        out = AUDIO_DIR / f.name
        if out != f:
            out.write_bytes(f.read_bytes())
            print(f"[copy] {f.name}")
        extracted += 1

print(f"Done. Processed {extracted} files.")
