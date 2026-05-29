# Security & authorized use — Mr. CrackBot AI

## Scope

This firmware exposes **real** Wi‑Fi and BLE lab capabilities for **authorized** security research on hardware you control.

## Authorized use only

- Networks and devices **you own**
- Assets covered by **written permission** or signed rules-of-engagement
- Isolated **lab VLAN** segments — never production or third-party networks without authorization

## You must not

- Deploy against neighbors, public Wi‑Fi, employers, or clients without explicit authorization
- Exfiltrate handshakes or credentials outside your lab retention policy
- Ship pre-flashed units without Hacker Planet LLC lab-use documentation

## Operator controls

- Change default portal AP password (`CRACKBOT_PORTAL_PASS` in `include/crackbot/config.h`) before field use
- SD exports may contain sensitive data — encrypt at rest
- USB serial export is enabled for lab automation

## Reporting

Security issues: **salvadorData@proton.me** (Hacker Planet LLC).

## Feature preservation policy

Hacker Planet does **not** remove attack-surface features for “safety through omission.” Distribution is gated by **lawful use documentation**, not feature stripping.
