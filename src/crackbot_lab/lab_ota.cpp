#include "crackbot/lab_ota.h"
#include "crackbot/ui_bridge.h"

#include <ArduinoOTA.h>

namespace crackbot::lab {

void setupOta(const char *hostname) {
  ArduinoOTA.setHostname(hostname);
  ArduinoOTA.onStart([]() { ui::setStatus("OTA start"); });
  ArduinoOTA.onEnd([]() { ui::setStatus("OTA end"); });
  ArduinoOTA.onError([](ota_error_t error) { ui::setStatus("OTA err " + String(error)); });
  ArduinoOTA.begin();
}

void handleOta() { ArduinoOTA.handle(); }

}  // namespace crackbot::lab
