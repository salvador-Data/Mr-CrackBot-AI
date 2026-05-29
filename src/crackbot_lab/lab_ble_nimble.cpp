#include "crackbot/config.h"
#include "crackbot/lab_ble.h"
#include "crackbot/lab_wifi.h"
#include "crackbot/state.h"
#include "crackbot/ui_bridge.h"

#include <NimBLEDevice.h>
#include <SD.h>
#include <esp_wifi.h>

namespace crackbot::lab {

void setupBleStack() { NimBLEDevice::init(CRACKBOT_HOSTNAME_CP); }

void scanBleDevices() {
  ui::setStatus("BLE scan 5s...");
  NimBLEScan *scan = NimBLEDevice::getScan();
  scan->start(5, false);
  ui::setStatus("BLE scan done");
}

void deployEavesdropping() {
  ui::setStatus("Eavesdrop deploy");
  scanBleDevices();
  File log = SD.open(CRACKBOT_EAVESDROP_LOG_PATH, FILE_APPEND);
  if (log) {
    log.printf("%lu eavesdrop_start\n", millis());
    log.close();
  }
}

void deployMitMAttack() { ui::setStatus("MitM lab service"); }

void deployBluetoothJamming() {
  ui::setStatus("RF jam pattern");
  setPromiscuousMode(true);
  for (int i = 0; i < 20; ++i) {
    esp_wifi_80211_tx(WIFI_IF_AP, gDeauthPacket, sizeof(gDeauthPacket), false);
    delay(30);
  }
  setPromiscuousMode(false);
}

void deployBluetoothSpoofing() {
  ui::setStatus("BLE spoof adv");
  NimBLEAdvertising *adv = NimBLEDevice::getAdvertising();
  adv->setName("MrCrackBot-Spoof");
  adv->start();
}

}  // namespace crackbot::lab
