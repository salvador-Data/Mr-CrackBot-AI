#include "crackbot_dict_attack.h"

#include "crackbot/ui_bridge.h"

namespace crackbot::dict_attack {

Result run(const Config &cfg, TryFn try_fn) {
  Result result;
  wordlist::Reader reader(cfg.list_set, cfg.filter);
  if (!reader.begin()) {
    ui::setStatus("No wordlist sources");
    return result;
  }

  ui::setStatus(String("Dict: ") + reader.activeLabel());
  String candidate;
  while (reader.next(candidate)) {
    result.attempts++;
    if (cfg.max_attempts && result.attempts > cfg.max_attempts) break;

    ui::progress("Try " + candidate.substring(0, min((unsigned)candidate.length(), 10u)));
    if (try_fn(candidate)) {
      result.success = true;
      result.password = candidate;
      result.winning_list = reader.activeList();
      result.list_label = reader.activeLabel();
      reader.close();
      return result;
    }
    if (cfg.delay_ms) delay(cfg.delay_ms);
  }

  reader.close();
  return result;
}

}  // namespace crackbot::dict_attack
