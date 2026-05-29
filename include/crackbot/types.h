#pragma once

#include <Arduino.h>
#include <vector>

namespace crackbot {

struct NetworkInfo {
  String ssid;
  String bssid;
  int rssi = 0;
  int channel = 0;
  bool has_password = false;
  bool pmf_enabled = false;
  String password;
  String encryption;
  String encryptionType;
  String vendor;
};

}  // namespace crackbot
