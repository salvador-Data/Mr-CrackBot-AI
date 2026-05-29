# M5 OS integration — Mr. CrackBot AI Cardputer build

The Cardputer firmware (`m5stack-cardputer` / `m5stack-crackbot` env) produces a standalone `.bin` suitable for the [M5_OS-Cardputer](https://github.com/salvadordata/M5_OS-Cardputer) app launcher model.

## Build artifact

```powershell
cd C:\Users\Owner\Projects\Mr-CrackBot-AI
py -3 -m platformio run -e m5stack-cardputer
```

Output: `.pio/build/m5stack-cardputer/firmware.bin`

## Install on M5 OS SD layout

Copy to the Cardputer microSD (FAT32):

```
/apps/crackbot/crackbot.bin
```

Add to `/apps/manifest.json` (or host manifest referenced by M5 OS):

```json
{
  "name": "Mr. CrackBot AI",
  "package": "crackbot",
  "bin": "/apps/crackbot/crackbot.bin",
  "version": "1.0.0",
  "author": "Hacker Planet LLC",
  "description": "Authorized Wi-Fi/BLE lab tool — keyboard UI"
}
```

Launch from M5 OS menu → **Launch installed apps** → `crackbot`.

## Keyboard controls

| Key | Action |
|-----|--------|
| `w` / `;` | Menu up |
| `s` / `.` | Menu down |
| Enter / Space | Select |
| `` ` `` / Esc | Back |
| `r` | Rescan (when Scan WiFi highlighted) |

## Notes

- M5 OS base firmware flash (M5Burner) is separate — see M5_OS-Cardputer `docs/APP_INSTALL.md`.
- CrackBot `.bin` is an **app**, not the M5 OS shell. Flash base OS first, then deploy this app to SD.
- Authorized lab VLAN only — see [SECURITY.md](../SECURITY.md).
