<p align="center">
  <img src="https://raw.githubusercontent.com/salvadordata/cyberThreatGotchi/main/docs/images/og-crackbot.png" alt="Mr. CrackBot AI — Hacker Planet lab stack" width="640"/>
</p>

<h1 align="center">Mr. CrackBot AI</h1>

<p align="center">
  <strong>Unified authorized Wi‑Fi &amp; BLE lab firmware — CYD touch + M5 Cardputer keyboard</strong>
</p>

<p align="center">
  <a href="https://github.com/salvadordata/Mr-CrackBot-AI/actions/workflows/build.yml">
    <img src="https://github.com/salvadordata/Mr-CrackBot-AI/actions/workflows/build.yml/badge.svg" alt="Build"/>
  </a>
  <a href="LICENSE"><img src="https://img.shields.io/badge/license-MIT-blue.svg" alt="MIT"/></a>
  <a href="https://salvador-Data.github.io/cyberThreatGotchi/crackbot.html">
    <img src="https://img.shields.io/badge/docs-Hacker%20Planet-00C2A8" alt="Product docs"/>
  </a>
  <img src="https://img.shields.io/badge/CYD-ESP32--2432S028-E7352C?logo=espressif&logoColor=white" alt="CYD"/>
  <img src="https://img.shields.io/badge/Cardputer-M5Stack-00C2FF" alt="Cardputer"/>
</p>

---

## About

**Hacker Planet LLC** · Philadelphia, PA — founded by salvadorData
This is the **canonical C++ firmware repository** for Mr. CrackBot AI field tools. One modular codebase builds for:

| Target | PlatformIO env | Hardware |
|--------|----------------|----------|
| **CYD** | `cyd-2432s028` | ESP32-2432S028 Cheap Yellow Display (2.8″ touch) |
| **M5 Cardputer** | `m5stack-cardputer` | M5Stack Cardputer (ESP32-S3, QWERTY) |

Jetson bench automation (Python, hashcat, Tk UI) remains in [Mr.-CrackBot-AI-Nano](https://github.com/salvadordata/Mr.-CrackBot-AI-Nano). Cardputer OS shell in [M5_OS-Cardputer](https://github.com/salvadordata/M5_OS-Cardputer).

Ecosystem → [CyberThreatGotchi](https://github.com/salvadordata/cyberThreatGotchi) · [Product page](https://salvador-Data.github.io/cyberThreatGotchi/crackbot.html)

## Features

- Wi‑Fi scan, select, vendor/encryption display
- WPA2 deauth + WPA3/PMF awareness
- Handshake / SAE promiscuous capture
- RockYou / rockyou2024 SD streaming + embedded lab subset (`crackbot_wordlist`)
- BLE scan, eavesdrop, MitM, jam/spoof patterns
- Captive lab portal + SD credential capture
- JSON export (SD + USB serial)
- OTA updates, battery readout, deep sleep
- **CYD:** touch grid UI · **Cardputer:** QWERTY menu parity

## Quick start

```powershell
cd C:\Users\Owner\Projects\Mr-CrackBot-AI
py -3 -m pip install platformio
py -3 -m platformio run -e cyd-2432s028
py -3 -m platformio run -e m5stack-cardputer
```

Flash CYD (replace `COM5`):

```powershell
py -3 -m platformio run -e cyd-2432s028 -t upload --upload-port COM5
```

Flash Cardputer:

```powershell
py -3 -m platformio run -e m5stack-cardputer -t upload --upload-port COM5
```

## Module layout

```
include/crackbot/          Shared headers (types, lab APIs, menu)
include/crackbot_wordlist.h  SD + PROGMEM wordlist streaming
src/crackbot_core/         Global state + UI bridge
src/crackbot_lab/          Wi‑Fi, BLE, portal, password, OTA (shared)
src/crackbot_wordlist.cpp  Wordlist reader + checkpoint
src/crackbot_ui/           Menu action dispatch
src/platform/cyd/          Touch UI + CYD hardware init
src/platform/cardputer/    Keyboard UI + M5 init
src/main.cpp               Thin entry point
```

Details → [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) · Wordlists → [docs/WORDLISTS.md](docs/WORDLISTS.md) · M5 OS app install → [docs/M5_OS_INTEGRATION.md](docs/M5_OS_INTEGRATION.md)

## Consolidated from

| Source | Status |
|--------|--------|
| `salvadordata/Mr.-CrackBot-AI-CYD` | Merged → modular `crackbot_lab/` + `platform/cyd/` |
| `Mr.-CrackBot-AI-Nano/cardputer/` | Merged → `platform/cardputer/` + shared lab |
| `_import/Mr-CRACKBOT-1.0` | Features in lab_wifi, lab_ble, lab_portal |
| `_import/MrCrackBotAIESP32CYD` | WPA3/PMF, TFLite hook in lab_password |
| `_import/M5-Cardputer-Mr.-Robot-Handshake-Keeper` | Handshake path in lab_wifi |

## Authorized use

**Authorized lab VLAN only** — networks you own or have written permission to test. See [SECURITY.md](SECURITY.md).

## License

MIT — Hacker Planet LLC. See [LICENSE](LICENSE).
