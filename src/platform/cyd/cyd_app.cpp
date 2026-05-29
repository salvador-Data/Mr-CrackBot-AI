#include "crackbot/config.h"
#include "crackbot/lab_ble.h"
#include "crackbot/lab_ota.h"
#include "crackbot/lab_portal.h"
#include "crackbot/lab_wifi.h"
#include "crackbot/menu.h"
#include "crackbot/state.h"
#include "crackbot/ui_bridge.h"

#include <BluetoothSerial.h>
#include <SD.h>
#include <TaskScheduler.h>
#include <TFT_eSPI.h>
#include <WiFiManager.h>
#include <XPT2046_Touchscreen.h>

using namespace crackbot;

static TFT_eSPI gTft = TFT_eSPI();
static XPT2046_Touchscreen gTouch(33, 36);
static Scheduler gSched;
static Task tBattery(60000, TASK_FOREVER, []() {
  int pct = lab::readBatteryPercent(34);
  gTft.fillRect(0, 0, 80, 12, TFT_BLACK);
  gTft.setCursor(0, 0);
  gTft.printf("Bat:%d%%", pct);
}, &gSched, true);
static BluetoothSerial gSerialBt;
static WiFiManager gWifiManager;

static void uiStatus(const String &msg) {
  gTft.fillRect(0, 200, 320, 20, TFT_BLACK);
  gTft.setCursor(0, 200);
  gTft.setTextSize(1);
  gTft.println(msg);
}

static void uiLine(const String &line) {
  gTft.setTextSize(1);
  gTft.println(line);
}

static void uiClear() {
  gTft.fillScreen(TFT_BLACK);
  gTft.setTextColor(TFT_WHITE, TFT_BLACK);
  gTft.setCursor(0, 16);
}

static void uiProgress(const String &msg) { uiStatus(msg); }

static void showIntro() {
  gTft.fillScreen(TFT_BLACK);
  gTft.setTextSize(2);
  gTft.setCursor(20, 100);
  gTft.setTextColor(TFT_RED, TFT_BLACK);
  gTft.println("Mr. CrackBot");
  gTft.setTextColor(TFT_WHITE, TFT_BLACK);
  gTft.setCursor(10, 130);
  gTft.println("AI CYD Lab");
  gTft.setCursor(10, 155);
  gTft.println("Hacker Planet");
  delay(1500);
  gTft.fillScreen(TFT_BLACK);
}

static void drawTouchMenu() {
  gTft.fillScreen(TFT_BLACK);
  gTft.setTextSize(1);
  gTft.setCursor(0, 0);
  gTft.println("Row1: Scan|Sel|Info|Pwn");
  gTft.println("Row2: Crack|Deauth|Set|BLE");
  gTft.println("Row3: Portal|Export|OTA|Sleep");
  for (int x = 0; x < 320; x += 80) {
    gTft.drawRect(x, 0, 80, 40, TFT_WHITE);
    gTft.drawRect(x, 40, 80, 40, TFT_CYAN);
  }
}

static void processTouch() {
  if (!gTouch.touched()) return;
  TS_Point p = gTouch.getPoint();
  uint16_t x = p.x;
  uint16_t y = p.y;

  if (y < 40) {
    if (x < 80) menu::run(menu::Action::ScanWifi);
    else if (x < 160) {
      int idx = p.y / 14;
      if (idx >= 0 && idx < (int)gNetworks.size()) lab::selectNetworkByIndex(idx);
    } else if (x < 240) menu::run(menu::Action::NetInfo);
    else menu::run(menu::Action::PwnHandshake);
  } else if (y < 80) {
    if (x < 80) menu::run(menu::Action::CrackWordlist);
    else if (x < 160) menu::run(menu::Action::Deauth);
    else if (x < 240) menu::run(menu::Action::SettingsPromiscuous);
    else menu::run(menu::Action::BleSecurity);
  } else if (y < 120) {
    if (x < 80) menu::run(menu::Action::EvilPortal);
    else if (x < 160) menu::run(menu::Action::ExportJson);
    else if (x < 240) lab::handleOta();
    else menu::run(menu::Action::DeepSleep);
  } else if (y > 200) {
    if (x < 80) menu::run(menu::Action::BleEavesdrop);
    else if (x < 160) menu::run(menu::Action::BleMitM);
    else if (x < 240) menu::run(menu::Action::BleJam);
    else menu::run(menu::Action::BleSpoof);
  }
}

void crackbotSetup() {
  Serial.begin(115200);
  gSerialBt.begin(CRACKBOT_HOSTNAME_CYD);
  gTft.init();
  gTft.setRotation(1);
  gTft.fillScreen(TFT_BLACK);
  gTouch.begin();
  gTouch.setRotation(1);

  ui::init(uiStatus, uiLine, uiClear, uiProgress);
  initDeauthTemplate();

  if (!SD.begin()) uiStatus("SD init failed");
  showIntro();
  lab::setupWifiStack();
  lab::setupBleStack();
  lab::setupPortal();
  lab::setupOta(CRACKBOT_HOSTNAME_CYD);

  gWifiManager.setConfigPortalTimeout(120);
  gWifiManager.autoConnect("MrCrackBot-CYD-Setup");

  gSched.addTask(tBattery);
  gSched.enableAll();
  drawTouchMenu();
}

void crackbotLoop() {
  gSched.execute();
  lab::handleOta();
  lab::handlePortalClients();
  processTouch();
  delay(5);
}
