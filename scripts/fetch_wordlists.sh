#!/usr/bin/env bash
# Copy or slice wordlists for Mr. CrackBot AI SD cards — authorized lab use only.
set -euo pipefail

KALI_CLASSIC="/usr/share/wordlists/rockyou.txt"
KALI_GZ="/usr/share/wordlists/rockyou.txt.gz"
SUBSET_SRC="data/wordlists/rockyou_lab_subset.txt"

usage() {
  echo "Usage:"
  echo "  $0 --classic DEST          Copy Kali rockyou.txt to DEST"
  echo "  $0 --gunzip-classic DEST   Gunzip Kali rockyou.txt.gz to DEST"
  echo "  $0 --subset SRC DEST [N]   Write first N valid lines (8-40 chars) to DEST"
  echo "  $0 --lab-subset DEST       Copy embedded lab subset to DEST"
  exit 1
}

filter_line() {
  local line="$1"
  local len=${#line}
  [[ $len -ge 8 && $len -le 40 ]] || return 1
  [[ "$line" =~ ^[[:print:]]+$ ]] || return 1
  return 0
}

[[ $# -ge 1 ]] || usage

case "${1:-}" in
  --classic)
    [[ $# -eq 2 ]] || usage
    if [[ ! -f "$KALI_CLASSIC" ]]; then
      echo "Missing $KALI_CLASSIC — run: sudo apt install wordlists" >&2
      exit 1
    fi
    cp "$KALI_CLASSIC" "$2"
    echo "Copied Kali rockyou.txt ($(wc -l < "$2") lines) -> $2"
    ;;
  --gunzip-classic)
    [[ $# -eq 2 ]] || usage
    if [[ ! -f "$KALI_GZ" ]]; then
      echo "Missing $KALI_GZ" >&2
      exit 1
    fi
    gunzip -c "$KALI_GZ" > "$2"
    echo "Wrote $(wc -l < "$2") lines -> $2"
    ;;
  --subset)
    [[ $# -ge 3 ]] || usage
    src="$2"
    dest="$3"
    max="${4:-50000}"
    count=0
    : > "$dest"
    while IFS= read -r line || [[ -n "$line" ]]; do
      line="${line//$'\r'/}"
      filter_line "$line" || continue
      printf '%s\n' "$line" >> "$dest"
      count=$((count + 1))
      [[ $count -ge $max ]] && break
    done < "$src"
    echo "Wrote $count lines -> $dest"
    ;;
  --lab-subset)
    [[ $# -eq 2 ]] || usage
    cp "$SUBSET_SRC" "$2"
    echo "Copied lab subset -> $2"
    ;;
  *)
    usage
    ;;
esac
