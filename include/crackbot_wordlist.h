#pragma once

/**
 * Mr. CrackBot AI — multi-list wordlist catalog (Kali + SD + embedded).
 * Hacker Planet LLC · salvadordata · authorized lab use only.
 */

#include <Arduino.h>
#include <FS.h>
#include <functional>

namespace crackbot::wordlist {

enum class ListId : uint8_t {
  LabSubset = 0,
  KaliRockyou,
  KaliRockyou2024Subset,
  KaliFasttrack,
  KaliDirbCommon,
  KaliNmapPasswords,
  KaliJohn,
  Count,
};

enum class SetId : uint8_t {
  LabOnly,
  KaliLabChain,
  KaliFullChain,
};

struct Filter {
  uint8_t min_length = 8;
  uint8_t max_length = 40;
  bool printable_ascii = true;
};

struct ListMeta {
  ListId id;
  const char *sd_path;
  const char *short_name;
  const char *kali_source;
  const char *typical_size;
  bool has_embedded;
};

const ListMeta &meta(ListId id);
const char *setLabel(SetId set);
size_t setListCount(SetId set);
ListId setListAt(SetId set, size_t index);

/** Stream one list, then optionally advance in a set. */
class Reader {
public:
  explicit Reader(SetId set = SetId::KaliLabChain, Filter filter = {});

  bool begin();
  void close();
  bool next(String &out);
  void reset();

  ListId activeList() const { return active_list_; }
  const char *activeLabel() const;
  size_t linesRead() const { return lines_read_; }

private:
  bool openList(ListId id);
  bool openNextInSet();
  bool nextFromSd(String &out);
  bool nextFromEmbedded(String &out);
  bool passesFilter(const String &line) const;
  bool loadCheckpoint();
  bool saveCheckpoint();

  SetId set_;
  Filter filter_;
  File sd_file_;
  ListId active_list_ = ListId::LabSubset;
  size_t set_index_ = 0;
  size_t lines_read_ = 0;
  size_t embed_idx_ = 0;
  bool embedded_active_ = false;
  bool list_exhausted_ = false;
};

using TryFn = std::function<bool(const String &password)>;
String crackWithWordlists(SetId set, TryFn try_fn, Filter filter = {});

const char *listAttribution(ListId id);

}  // namespace crackbot::wordlist
