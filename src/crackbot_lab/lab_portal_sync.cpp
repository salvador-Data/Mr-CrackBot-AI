#include "crackbot/config.h"
#include "crackbot/lab_portal.h"
#include "crackbot/ui_bridge.h"

#include <SD.h>
#include <WebServer.h>
#include <WiFi.h>

namespace crackbot::lab {

static WebServer *gPortal = nullptr;

void setupPortal() {
  if (gPortal) return;
  WiFi.softAP(CRACKBOT_HOSTNAME_CP, CRACKBOT_PORTAL_PASS);
  gPortal = new WebServer(80);
  gPortal->on("/", []() {
    gPortal->send(200, "text/html",
                  "<h1>CrackBot Lab Portal</h1><p>Authorized lab only.</p>"
                  "<form method='POST' action='/save'>SSID<input name='ssid'/>"
                  "<input type='submit'/></form>");
  });
  gPortal->on("/save", HTTP_POST, []() {
    if (gPortal->hasArg("ssid")) {
      String ssid = gPortal->arg("ssid");
      File f = SD.open(CRACKBOT_PORTAL_CREDS_PATH, FILE_WRITE);
      if (f) {
        f.printf("{\"ssid\":\"%s\"}\n", ssid.c_str());
        f.close();
      }
    }
    gPortal->send(200, "text/plain", "Saved to SD (lab)");
  });
  gPortal->begin();
}

void startPortal() {
  setupPortal();
  ui::setStatus("Portal on");
}

void stopPortal() {
  if (gPortal) {
    gPortal->stop();
    WiFi.softAPdisconnect(true);
    ui::setStatus("Portal off");
  }
}

void handlePortalClients() {
  if (gPortal) gPortal->handleClient();
}

}  // namespace crackbot::lab
