#include "crackbot_wordlist.h"

#include "crackbot/config.h"
#include "crackbot/ui_bridge.h"

#include <SD.h>
#include <pgmspace.h>

namespace crackbot::wordlist {

namespace {

static const char kLabSubset[] PROGMEM = R"LAB(
password
123456
12345678
123456789
1234567890
qwerty
abc123
password1
admin
letmein
welcome
monkey
dragon
master
login
princess
football
shadow
sunshine
trustno1
iloveyou
batman
access
hello
charlie
donald
password123
qwerty123
welcome1
admin123
Password1
P@ssw0rd
changeme
default
guest
root
toor
passw0rd
secret
test1234
labvlanonly
hackerplanet
)LAB";

static const ListMeta kCatalog[] = {
    {ListId::LabSubset, nullptr, "lab-subset", "Embedded (data/wordlists/)", "<1 KB", true},
    {ListId::KaliRockyou, CRACKBOT_ROCKYOU_CLASSIC_PATH, "rockyou", "Kali /usr/share/wordlists/rockyou.txt.gz", "~134 MB", false},
    {ListId::KaliRockyou2024Subset, CRACKBOT_ROCKYOU2024_ALT_PATH, "rockyou2024", "Operator SD subset (not 160GB dump)", "subset", false},
    {ListId::KaliFasttrack, CRACKBOT_FASTTRACK_PATH, "fasttrack", "Kali fasttrack.txt symlink", "~small", false},
    {ListId::KaliDirbCommon, CRACKBOT_DIRB_COMMON_PATH, "dirb-common", "Kali dirb/common.txt", "~small", false},
    {ListId::KaliNmapPasswords, CRACKBOT_NMAP_PASSWORDS_PATH, "nmap.lst", "Kali nmap.lst", "~small", false},
    {ListId::KaliJohn, CRACKBOT_JOHN_PATH, "john.lst", "Kali john.lst", "~small", false},
};

static const ListId kSetLabOnly[] = {ListId::LabSubset};
static const ListId kSetKaliLab[] = {ListId::LabSubset, ListId::KaliFasttrack, ListId::KaliDirbCommon,
                                    ListId::KaliNmapPasswords, ListId::KaliJohn, ListId::KaliRockyou,
                                    ListId::KaliRockyou2024Subset};
static const ListId kSetKaliFull[] = {ListId::KaliRockyou, ListId::KaliRockyou2024Subset, ListId::KaliFasttrack,
                                      ListId::KaliDirbCommon, ListId::KaliNmapPasswords, ListId::KaliJohn,
                                      ListId::LabSubset};

String trimLine(const String &raw) {
  String line = raw;
  line.trim();
  return line;
}

const char *pathFor(ListId id) {
  if ((size_t)id >= (size_t)ListId::Count) return nullptr;
  return kCatalog[(size_t)id].sd_path;
}

const ListId *listsFor(SetId set, size_t &count) {
  switch (set) {
    case SetId::LabOnly:
      count = sizeof(kSetLabOnly);
      return kSetLabOnly;
    case SetId::KaliLabChain:
      count = sizeof(kSetKaliLab);
      return kSetKaliLab;
    case SetId::KaliFullChain:
      count = sizeof(kSetKaliFull);
      return kSetKaliFull;
  }
  count = 0;
  return nullptr;
}

}  // namespace

const ListMeta &meta(ListId id) {
  static ListMeta unknown{ListId::LabSubset, nullptr, "?", "?", "?", false};
  if ((size_t)id >= (size_t)ListId::Count) return unknown;
  return kCatalog[(size_t)id];
}

const char *setLabel(SetId set) {
  switch (set) {
    case SetId::LabOnly: return "Lab subset only";
    case SetId::KaliLabChain: return "Kali lab chain";
    case SetId::KaliFullChain: return "Kali full chain";
  }
  return "unknown";
}

size_t setListCount(SetId set) {
  size_t count = 0;
  listsFor(set, count);
  return count / sizeof(ListId);
}

ListId setListAt(SetId set, size_t index) {
  size_t count = 0;
  const ListId *lists = listsFor(set, count);
  count /= sizeof(ListId);
  if (!lists || index >= count) return ListId::LabSubset;
  return lists[index];
}

const char *listAttribution(ListId id) { return meta(id).kali_source; }

Reader::Reader(SetId set, Filter filter) : set_(set), filter_(filter) {}

bool Reader::passesFilter(const String &line) const {
  if (line.length() < filter_.min_length || line.length() > filter_.max_length) return false;
  if (!filter_.printable_ascii) return true;
  for (size_t i = 0; i < line.length(); ++i) {
    char c = line.charAt(i);
    if (c < 32 || c > 126) return false;
  }
  return true;
}

bool Reader::loadCheckpoint() {
  File cp = SD.open(CRACKBOT_CHECKPOINT_PATH, FILE_READ);
  if (!cp) return false;
  String val = cp.readStringUntil('\n');
  cp.close();
  return val.length() > 0;
}

bool Reader::saveCheckpoint() {
  if (!sd_file_) return false;
  File cp = SD.open(CRACKBOT_CHECKPOINT_PATH, FILE_WRITE);
  if (!cp) return false;
  cp.printf("%u:%lu\n", (unsigned)active_list_, (unsigned long)sd_file_.position());
  cp.close();
  return true;
}

bool Reader::openList(ListId id) {
  if (sd_file_) {
    sd_file_.close();
    sd_file_ = File();
  }
  embedded_active_ = false;
  list_exhausted_ = false;
  active_list_ = id;
  embed_idx_ = 0;

  if (meta(id).has_embedded) {
    embedded_active_ = true;
    return true;
  }

  const char *path = pathFor(id);
  if (!path || !SD.exists(path)) {
    if (id == ListId::KaliRockyou2024Subset && SD.exists(CRACKBOT_ROCKYOU2024_PATH)) {
      path = CRACKBOT_ROCKYOU2024_PATH;
    } else {
      return false;
    }
  }
  sd_file_ = SD.open(path, FILE_READ);
  return (bool)sd_file_;
}

bool Reader::openNextInSet() {
  while (set_index_ < setListCount(set_)) {
    ListId id = setListAt(set_, set_index_++);
    if (openList(id)) return true;
  }
  return false;
}

bool Reader::begin() {
  close();
  set_index_ = 0;
  lines_read_ = 0;
  loadCheckpoint();
  return openNextInSet() || openList(ListId::LabSubset);
}

void Reader::close() {
  if (sd_file_) {
    saveCheckpoint();
    sd_file_.close();
  }
  embedded_active_ = false;
}

void Reader::reset() {
  embed_idx_ = 0;
  lines_read_ = 0;
  set_index_ = 0;
  if (sd_file_) sd_file_.seek(0);
  if (SD.exists(CRACKBOT_CHECKPOINT_PATH)) SD.remove(CRACKBOT_CHECKPOINT_PATH);
}

const char *Reader::activeLabel() const { return meta(active_list_).short_name; }

bool Reader::nextFromEmbedded(String &out) {
  while (true) {
    int start = embed_idx_;
    if (kLabSubset[start] == '\0') return false;
    while (kLabSubset[embed_idx_] != '\0' && kLabSubset[embed_idx_] != '\n') embed_idx_++;
    char buf[64] = {0};
    size_t len = embed_idx_ - start;
    if (len >= sizeof(buf)) len = sizeof(buf) - 1;
    memcpy_P(buf, kLabSubset + start, len);
    embed_idx_++;
    if (kLabSubset[embed_idx_] == '\n') embed_idx_++;
    out = trimLine(String(buf));
    if (out.length() && passesFilter(out)) {
      lines_read_++;
      return true;
    }
  }
}

bool Reader::nextFromSd(String &out) {
  while (sd_file_.available()) {
    String line = trimLine(sd_file_.readStringUntil('\n'));
    if (lines_read_ % 128 == 0) saveCheckpoint();
    if (!line.length()) continue;
    if (!passesFilter(line)) continue;
    out = line;
    lines_read_++;
    return true;
  }
  sd_file_.close();
  sd_file_ = File();
  return false;
}

bool Reader::next(String &out) {
  while (true) {
    if (embedded_active_) {
      if (nextFromEmbedded(out)) return true;
      embedded_active_ = false;
      if (!openNextInSet()) return false;
      continue;
    }
    if (sd_file_) {
      if (nextFromSd(out)) return true;
      if (!openNextInSet()) return false;
      continue;
    }
    if (!openNextInSet()) return false;
  }
}

String crackWithWordlists(SetId set, TryFn try_fn, Filter filter) {
  Reader reader(set, filter);
  if (!reader.begin()) {
    ui::setStatus("No wordlist source");
    return "";
  }
  ui::setStatus(String("Wordlist: ") + reader.activeLabel());
  String candidate;
  while (reader.next(candidate)) {
    ui::progress("Trying " + candidate.substring(0, min((unsigned)candidate.length(), 12u)) + "...");
    if (try_fn(candidate)) {
      reader.close();
      return candidate;
    }
  }
  reader.close();
  return "";
}

}  // namespace crackbot::wordlist
