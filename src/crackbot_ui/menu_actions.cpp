#include "crackbot/config.h"
#include "crackbot/lab_ble.h"
#include "crackbot/lab_ota.h"
#include "crackbot/lab_portal.h"
#include "crackbot/lab_wifi.h"
#include "crackbot/menu.h"
#include "crackbot/state.h"
#include "crackbot/ui_bridge.h"

namespace crackbot::menu {

const char *actionLabel(Action action) {
  switch (action) {
    case Action::ScanWifi: return "Scan WiFi";
    case Action::SelectNet: return "Select net";
    case Action::NetInfo: return "Net info";
    case Action::PwnHandshake: return "Pwn/handshake";
    case Action::CrackWordlist: return "Crack wordlist";
    case Action::Deauth: return "Deauth";
    case Action::BleSecurity: return "BLE security";
    case Action::EvilPortal: return "Evil portal";
    case Action::ExportJson: return "Export JSON";
    case Action::Promiscuous: return "Promiscuous";
    case Action::Ota: return "OTA";
    case Action::DeepSleep: return "Deep sleep";
    default: return "Lab action";
  }
}

void run(Action action) {
  switch (action) {
    case Action::ScanWifi:
      lab::scanNetworks();
      break;
    case Action::SelectNet:
      if (!gNetworks.empty()) {
        lab::selectNetworkByIndex(0);
        ui::setStatus(gSelected.ssid);
      } else {
        ui::setStatus("Scan first");
      }
      break;
    case Action::NetInfo:
      if (gSelected.ssid.length()) lab::displayNetworkInfo(gSelected);
      else ui::setStatus("No selection");
      break;
    case Action::PwnHandshake:
      lab::captureHandshake();
      break;
    case Action::CrackWordlist:
      lab::crackSelected();
      break;
    case Action::Deauth:
      lab::deauthSelected();
      break;
    case Action::BleSecurity:
      lab::scanBleDevices();
      break;
    case Action::EvilPortal:
      lab::startPortal();
      break;
    case Action::ExportJson:
      lab::exportNetworksJson(true);
      break;
    case Action::Promiscuous:
    case Action::SettingsPromiscuous:
      lab::togglePromiscuousMode();
      break;
    case Action::Ota:
      ui::setStatus("OTA ready (USB/WiFi)");
      break;
    case Action::DeepSleep:
      lab::enterDeepSleep();
      break;
    case Action::BleEavesdrop:
      lab::deployEavesdropping();
      break;
    case Action::BleMitM:
      lab::deployMitMAttack();
      break;
    case Action::BleJam:
      lab::deployBluetoothJamming();
      break;
    case Action::BleSpoof:
      lab::deployBluetoothSpoofing();
      break;
    case Action::SettingsReset:
      lab::resetNetworkSettings();
      break;
    case Action::StopPortal:
      lab::stopPortal();
      break;
    case Action::SettingsBrightness:
      ledcSetup(7, 5000, 8);
      ledcAttachPin(21, 7);
      ledcWrite(7, 200);
      ui::setStatus("Brightness 200");
      break;
  }
}

}  // namespace crackbot::menu
