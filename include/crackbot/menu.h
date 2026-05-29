#pragma once

namespace crackbot::menu {

enum class Action {
  ScanWifi,
  SelectNet,
  NetInfo,
  PwnHandshake,
  CrackWordlist,
  Deauth,
  BleSecurity,
  EvilPortal,
  ExportJson,
  Promiscuous,
  Ota,
  DeepSleep,
  BleEavesdrop,
  BleMitM,
  BleJam,
  BleSpoof,
  SettingsPromiscuous,
  SettingsBrightness,
  SettingsReset,
  StopPortal,
};

const char *actionLabel(Action action);
void run(Action action);

}  // namespace crackbot::menu
