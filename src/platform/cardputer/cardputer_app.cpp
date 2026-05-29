#include "crackbot/config.h"
#include "crackbot/lab_ble.h"
#include "crackbot/lab_ota.h"
#include "crackbot/lab_portal.h"
#include "crackbot/lab_wifi.h"
#include "crackbot/menu.h"
#include "crackbot/state.h"
#include "crackbot/ui_bridge.h"

#include <M5Cardputer.h>
#include <SD.h>
#include <TaskScheduler.h>
#include <WiFi.h>

using namespace crackbot;

static const menu::Action kMainMenu[] = {
    menu::Action::ScanWifi,     menu::Action::SelectNet,   menu::Action::NetInfo,
    menu::Action::PwnHandshake, menu::Action::CrackWordlist, menu::Action::Deauth,
    menu::Action::BleSecurity,  menu::Action::EvilPortal,  menu::Action::ExportJson,
    menu::Action::Promiscuous,  menu::Action::Ota,         menu::Action::DeepSleep};
static const int kMenuCount = 12;
static int gMenuIdx = 0;
static int gScroll = 0;
static String gStatus = "Authorized lab only";
static Scheduler gSched;

static Task tBattery(60000, TASK_FOREVER, []() {
  int pct = lab::readBatteryPercent(4);
  gStatus = "Bat " + String(pct) + "%";
}, &gSched, true);

static void uiStatus(const String &msg) { gStatus = msg; }

static void uiLine(const String &line) {
  auto &d = M5Cardputer.Display;
  d.println(line);
}

static void uiClear() {
  M5Cardputer.Display.fillScreen(BLACK);
  M5Cardputer.Display.setCursor(0, 0);
}

static void drawMenu() {
  auto &d = M5Cardputer.Display;
  d.fillScreen(BLACK);
  d.setTextSize(1);
  d.setCursor(0, 0);
  d.println("Mr. CrackBot Cardputer");
  d.println(gStatus);
  for (int i = 0; i < 6; ++i) {
    int idx = gScroll + i;
    if (idx >= kMenuCount) break;
    if (idx == gMenuIdx) d.print("> ");
    else d.print("  ");
    d.println(menu::actionLabel(kMainMenu[idx]));
  }
}

static bool readKeys(bool &up, bool &down, bool &ok, bool &back, bool &act) {
  up = down = ok = back = act = false;
  if (!M5Cardputer.Keyboard.isChange() || !M5Cardputer.Keyboard.isPressed()) return false;
  auto st = M5Cardputer.Keyboard.keysState();
  for (auto c : st.word) {
    if (c == ';' || c == 'w' || c == 'W') up = true;
    if (c == '.' || c == 's' || c == 'S') down = true;
    if (c == '\n' || c == ' ') ok = true;
    if (c == '`' || c == 27) back = true;
    if (c == 'r' || c == 'R') act = true;
  }
  return true;
}

void crackbotSetup() {
  auto cfg = M5.config();
  M5Cardputer.begin(cfg, true);
  M5Cardputer.Display.setRotation(1);
  Serial.begin(115200);

  ui::init(uiStatus, uiLine, uiClear, uiStatus);
  initDeauthTemplate();

  WiFi.mode(WIFI_STA);
  SD.begin();
  lab::setupWifiStack();
  lab::setupBleStack();
  lab::setupOta(CRACKBOT_HOSTNAME_CP);

  gSched.addTask(tBattery);
  gSched.enableAll();
  drawMenu();
}

void crackbotLoop() {
  gSched.execute();
  lab::handleOta();
  lab::handlePortalClients();

  bool up, down, ok, back, act;
  if (!readKeys(up, down, ok, back, act)) return;

  if (up) gMenuIdx = (gMenuIdx - 1 + kMenuCount) % kMenuCount;
  if (down) gMenuIdx = (gMenuIdx + 1) % kMenuCount;
  if (gMenuIdx < gScroll) gScroll = gMenuIdx;
  if (gMenuIdx >= gScroll + 6) gScroll = gMenuIdx - 5;

  if (act && kMainMenu[gMenuIdx] == menu::Action::ScanWifi) menu::run(menu::Action::ScanWifi);
  if (ok) menu::run(kMainMenu[gMenuIdx]);

  drawMenu();
  delay(80);
}
