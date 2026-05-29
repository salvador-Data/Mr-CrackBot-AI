#include "crackbot/config.h"
#include "crackbot/lab_portal.h"
#include "crackbot/ui_bridge.h"

#include <ESPAsyncWebServer.h>
#include <SD.h>
#include <WiFi.h>

namespace crackbot::lab {

static AsyncWebServer *gPortal = nullptr;
static bool gPortalRunning = false;

void setupPortal() {
  if (gPortal) return;
  gPortal = new AsyncWebServer(80);
  gPortal->on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/html",
                  "<html><body><h1>Mr. CrackBot Lab Portal</h1>"
                  "<p>Authorized testing VLAN only.</p>"
                  "<form method='POST' action='/save'>SSID<input name='ssid'/>"
                  "<input type='submit'/></form></body></html>");
  });
  gPortal->on("/save", HTTP_POST, [](AsyncWebServerRequest *request) {
    if (request->hasParam("ssid", true)) {
      String ssid = request->getParam("ssid", true)->value();
      File f = SD.open(CRACKBOT_PORTAL_CREDS_PATH, FILE_WRITE);
      if (f) {
        f.printf("{\"ssid\":\"%s\"}\n", ssid.c_str());
        f.close();
      }
    }
    request->send(200, "text/plain", "Saved to SD (lab)");
  });
  gPortal->onNotFound([](AsyncWebServerRequest *request) {
    request->redirect("http://192.168.4.1/");
  });
}

void startPortal() {
  setupPortal();
  if (!gPortalRunning && gPortal) {
    WiFi.softAP(CRACKBOT_PORTAL_SSID, CRACKBOT_PORTAL_PASS);
    gPortal->begin();
    gPortalRunning = true;
    ui::setStatus("Portal AP on");
  }
}

void stopPortal() {
  if (gPortal && gPortalRunning) {
    gPortal->end();
    gPortalRunning = false;
    WiFi.softAPdisconnect(true);
    ui::setStatus("Portal off");
  }
}

void handlePortalClients() {}

}  // namespace crackbot::lab
