#pragma once

#include "crackbot_wordlist.h"

#include <functional>

namespace crackbot::dict_attack {

using TryFn = std::function<bool(const String &password)>;

struct Config {
  wordlist::SetId list_set = wordlist::SetId::KaliLabChain;
  uint16_t delay_ms = 150;
  uint32_t max_attempts = 0;  ///< 0 = unlimited
  bool stop_on_success = true;
  wordlist::Filter filter{};
};

struct Result {
  bool success = false;
  String password;
  size_t attempts = 0;
  wordlist::ListId winning_list = wordlist::ListId::LabSubset;
  const char *list_label = "";
};

/** Authorized lab dictionary attack — iterates chained wordlists with rate limit. */
Result run(const Config &cfg, TryFn try_fn);

}  // namespace crackbot::dict_attack
