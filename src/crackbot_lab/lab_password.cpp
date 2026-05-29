#include "crackbot/config.h"
#include "crackbot/lab_password.h"
#include "crackbot/lab_wifi.h"
#include "crackbot/state.h"
#include "crackbot/ui_bridge.h"
#include "crackbot_dict_attack.h"

namespace crackbot::lab {

void generateHeuristicPasswords(const String &ssid, const String &bssid, std::vector<String> &out) {
  String slug = ssid;
  slug.replace(" ", "");
  slug.toLowerCase();
  if (!slug.length()) slug = "network";

  String prefix = bssid;
  prefix.replace(":", "");
  prefix = prefix.substring(0, 6);

  const char *trials[] = {"trial", "admin", "default", "hello", "network"};
  out.push_back(slug + "1234");
  out.push_back(slug + "2024!");
  out.push_back(slug + "-admin1");
  out.push_back("Welcome" + slug + "1");
  out.push_back(prefix + slug + "1");
  out.push_back(slug + "@home123");

  for (const char *w1 : trials) {
    for (int n = 1; n < 4; ++n) {
      for (const char *w2 : trials) {
        String p = String(w1) + "-" + w2 + String(n) + "-" + w2;
        if (p.length() >= 8) out.push_back(p);
      }
    }
  }
}

String generateAiPasswordGuess(const String &ssid, const String &bssid) {
#ifdef MR_CRACKBOT_TFLITE
  (void)bssid;
  return ssid + String((int)(ssid.length() + bssid.length()));
#else
  int score = ssid.length() + bssid.length();
  return ssid + String(score % 1000);
#endif
}

String crackPasswordBlocking(const String &ssid, const String &bssid) {
  std::vector<String> guesses;
  generateHeuristicPasswords(ssid, bssid, guesses);
  String ai = generateAiPasswordGuess(ssid, bssid);
  if (ai.length()) guesses.push_back(ai);

  for (const auto &g : guesses) {
    ui::progress("Trying heuristic...");
    if (tryPassword(ssid, bssid, g)) return g;
  }

  dict_attack::Config cfg;
  cfg.list_set = wordlist::SetId::KaliLabChain;
  cfg.delay_ms = 150;
  cfg.stop_on_success = true;
  cfg.filter.min_length = 8;
  cfg.filter.max_length = 40;

  dict_attack::Result r = dict_attack::run(cfg, [&](const String &pw) {
    return tryPassword(ssid, bssid, pw);
  });
  return r.success ? r.password : String("");
}

}  // namespace crackbot::lab
