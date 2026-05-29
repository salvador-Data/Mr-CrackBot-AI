#pragma once

#include "crackbot/types.h"

#include <vector>

namespace crackbot::lab {

String vendorFromBssid(const String &bssid);
void loadNetworksFromSd();
void saveNetworksToSd();
void exportNetworksJson(bool toSerial);
void scanNetworks();
void displayNetworkInfo(const NetworkInfo &net);
void selectNetworkByIndex(int idx);
void fillDeauthPacket(const String &bssid, bool broadcast);
void sendDeauthPackets(const String &bssid, int count, bool broadcast);
void setPromiscuousMode(bool enable);
void deauthSelected();
void captureHandshake();
void crackSelected();
bool tryPassword(const String &ssid, const String &bssid, const String &password);
void setupWifiStack();
void resetNetworkSettings();
void enterDeepSleep();
void togglePromiscuousMode();
int readBatteryPercent(int adcPin);

}  // namespace crackbot::lab
