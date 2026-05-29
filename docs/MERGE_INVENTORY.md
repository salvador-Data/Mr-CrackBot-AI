# Merge inventory — Andy's CrackBot work → Mr-CrackBot-AI

Unified canonical repo: **https://github.com/salvadordata/Mr-CrackBot-AI**

| Source repo / path | Feature | Merged module |
|--------------------|---------|---------------|
| `Mr.-CrackBot-AI-CYD` (`wifi_lab`, `ble_lab`, `portal_lab`, `ui_touch`, `ota_update`, `password_gen`) | Wi‑Fi scan/deauth/handshake, BLE lab, captive portal, touch UI, OTA, heuristics | `src/crackbot_lab/*`, `src/platform/cyd/cyd_app.cpp` |
| `Mr.-CrackBot-AI-Nano/cardputer/` | QWERTY menu parity | `src/platform/cardputer/cardputer_app.cpp` |
| `_import/Mr-CRACKBOT-1.0` | Vendor table, JSON export, deauth logs, portal | `lab_wifi.cpp`, `lab_portal_async.cpp`, `lab_ble_esp.cpp` |
| `_import/MrCrackBotAIESP32CYD` | WPA3/PMF guard, TFLite hook | `lab_wifi.cpp`, `lab_password.cpp` (`MR_CRACKBOT_TFLITE`) |
| `_import/M5-Cardputer-Mr.-Robot-Handshake-Keeper` | Handshake capture path | `lab_wifi.cpp` `captureHandshake()` |
| `Mr.-CrackBot-AI-Nano` Python (`ai/password_model.py`, `setup.py`) | Jetson bench wordlists | **Stays in Nano repo** — linked from README |
| `cyberThreatGotchi/scripts/mr_crackbot/` | Heuristic wordlist generator | Patterns in `lab_password.cpp` |
| New (this repo) | Multi Kali SD lists + dict attack | `crackbot_wordlist`, `crackbot_dict_attack` |

## Not duplicated (by design)

- Jetson hashcat / Tk UI → [Mr.-CrackBot-AI-Nano](https://github.com/salvadordata/Mr.-CrackBot-AI-Nano)
- M5 OS shell → [M5_OS-Cardputer](https://github.com/salvadordata/M5_OS-Cardputer)
- 3D STLs → Nano `hardware/`

## Supersedes

- `salvadordata/Mr.-CrackBot-AI-CYD` — CYD firmware now built from this repo env `cyd-2432s028`
