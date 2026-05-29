# Wordlists — Mr. CrackBot AI

**Authorized lab use only.** Use wordlists only on networks and accounts you own or have written permission to test inside an isolated lab VLAN.

## What Kali ships (official)

| Asset | Kali path | Size | In this repo? |
|-------|-----------|------|---------------|
| **rockyou.txt.gz** | `/usr/share/wordlists/rockyou.txt.gz` | **51 MB** compressed | No — copy to SD |
| **rockyou.txt** (gunzip) | `/usr/share/wordlists/rockyou.txt` | **~134 MB**, ~14.3M lines | No — copy to SD |
| **rockyou2024.txt** | *Not in Kali wordlists package* | **~160 GB** raw leak dump | **Never** commit or flash whole file |

Kali documentation: [wordlists tool page](https://www.kali.org/tools/wordlists/)

Install on Kali:

```bash
sudo apt install wordlists
gunzip -k /usr/share/wordlists/rockyou.txt.gz
```

## rockyou2024.txt — important size note

`rockyou2024.txt` is **not** the small Kali `rockyou.txt`. Public references describe:

| Variant | Approx. size | Notes |
|---------|--------------|-------|
| Raw `rockyou2024.txt` | ~160 GB / ~10B lines | Unusable on ESP32; mostly low-quality compilation |
| `rockyou2024.zip` | ~50 GB | Torrent/magnet sources — bench Jetson only |
| Printable cleaned subset | ~25 GB | Third-party filtered builds — still SD/bench only |

**Mr.-CrackBot-AI firmware (CYD / Cardputer)** streams from **microSD** with checkpointing. Copy only a **lab subset** to the card as `/rockyou2024.txt` if you need that filename — do not attempt the full 160 GB file on device.

Jetson bench downloads: [Mr.-CrackBot-AI-Nano](https://github.com/salvadordata/Mr.-CrackBot-AI-Nano) `setup.py` (optional Mega archives).

## SD layout (FAT32)

| Path | Purpose |
|------|---------|
| `/rockyou.txt` | Classic Kali rockyou (recommended for field devices) |
| `/rockyou2024.txt` | Operator-provided **subset** (optional) |
| `/wordlists/rockyou2024.txt` | Alternate path (checked automatically) |
| `/checkpoint.txt` | Wordlist resume offset (auto-written) |

## Embedded lab subset

`data/wordlists/rockyou_lab_subset.txt` (~40 lines) ships in-repo for **authorized training demos** when no SD wordlist is present. It is compiled into firmware via `crackbot_wordlist.cpp` (PROGMEM).

## Fetch helpers (bench / SD prep)

**Windows (PowerShell)** — after installing Kali/WSL or copying from a Kali VM:

```powershell
cd C:\Users\Owner\Projects\Mr-CrackBot-AI
.\scripts\fetch_wordlists.ps1 -Source kali-classic -Dest D:\sd_mount\rockyou.txt
```

**Linux / Kali**:

```bash
./scripts/fetch_wordlists.sh --classic /mnt/sd/rockyou.txt
```

To build a small rockyou2024 **lab slice** from a bench copy (first N valid lines):

```bash
./scripts/fetch_wordlists.sh --subset /path/to/rockyou2024.txt /mnt/sd/rockyou2024.txt 50000
```

## Module API

| File | Role |
|------|------|
| `include/crackbot_wordlist.h` | `wordlist::Reader`, `crackWithWordlists()` |
| `src/crackbot_wordlist.cpp` | SD streaming, checkpoint, PROGMEM subset |
| `src/crackbot_lab/lab_password.cpp` | Calls wordlist module during crack |

## Attribution

- **rockyou.txt** — bundled with Kali `wordlists` package; derived from the 2009 RockYou breach corpus used for authorized security research.
- **rockyou2024** — third-party compilation (2024); not curated by Kali or Hacker Planet LLC. Use only for authorized credential-strength testing in your lab.

## Legal

Educational and authorized testing only. Hacker Planet LLC is not responsible for misuse.
