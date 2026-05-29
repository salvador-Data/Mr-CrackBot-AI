#pragma once

#include <Arduino.h>
#include <vector>

namespace crackbot::lab {

void generateHeuristicPasswords(const String &ssid, const String &bssid, std::vector<String> &out);
String generateAiPasswordGuess(const String &ssid, const String &bssid);
String crackPasswordBlocking(const String &ssid, const String &bssid);

}  // namespace crackbot::lab
