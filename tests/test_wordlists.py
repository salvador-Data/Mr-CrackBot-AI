"""Host-side tests for Mr-CrackBot-AI wordlist assets and fetch scripts."""

from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
SUBSET = ROOT / "data" / "wordlists" / "rockyou_lab_subset.txt"


def test_lab_subset_exists_and_nonempty():
    assert SUBSET.is_file()
    lines = [ln.strip() for ln in SUBSET.read_text(encoding="utf-8").splitlines() if ln.strip()]
    assert len(lines) >= 10


def test_lab_subset_password_lengths_for_filter():
    lines = [ln.strip() for ln in SUBSET.read_text(encoding="utf-8").splitlines() if ln.strip()]
    valid = [ln for ln in lines if 8 <= len(ln) <= 40 and ln.isascii()]
    assert len(valid) >= 5


def test_fetch_scripts_exist():
    assert (ROOT / "scripts" / "fetch_wordlists.sh").is_file()
    assert (ROOT / "scripts" / "fetch_wordlists.ps1").is_file()


def test_wordlist_docs_exist():
    assert (ROOT / "docs" / "WORDLISTS.md").is_file()
    text = (ROOT / "docs" / "WORDLISTS.md").read_text(encoding="utf-8")
    assert "rockyou.txt" in text
    assert "authorized" in text.lower()


def test_platformio_envs():
    ini = (ROOT / "platformio.ini").read_text(encoding="utf-8")
    assert "cyd-2432s028" in ini
    assert "m5stack-cardputer" in ini
