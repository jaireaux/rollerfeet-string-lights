#include <Arduino.h>
#include <ArduinoOTA.h>
#include <WiFi.h>
#include <WebServer.h>
#include "version.h"
#include "wifi_retry.h"
#include "ota_service.h"
#include "light_control.h"
#include "web_app.h"
#if __has_include("arduino_secrets.h")
#include "arduino_secrets.h"
#else
#include "arduino_secrets.example.h"
#endif
#if __has_include("ota_secrets.h")
#include "ota_secrets.h"
#else
#include "ota_secrets.example.h"
#endif

namespace {
constexpr char hostname[] = "holiday-lights";
struct Network { const char *ssid; const char *password; };
const Network networks[] = {
  {SECRET_WIFI_SSID, SECRET_WIFI_PASSWORD},
  {SECRET_WIFI_SSID_2, SECRET_WIFI_PASSWORD_2}
};
const uint8_t networkCount = SECRET_WIFI_SSID_2[0] && SECRET_WIFI_PASSWORD_2[0] ? 2 : 1;
WiFiRetry retry(networkCount);
WebServer statusServer(80);
bool enabled = false, ready = false, updating = false, restartAnimation = false;
unsigned lastPercent = 101;
StatusIndicator indicator;
void (*statusFrameCallback)(uint32_t) = nullptr;
bool rebootPending = false;
uint32_t completedAt = 0;

void refreshStatus() {
  if (statusFrameCallback) statusFrameCallback(millis());
}

void sendStatus() {
  String body = "{\"version\":\"" HOLIDAY_LIGHTS_VERSION "\",\"hostname\":\"";
  body += hostname;
  body += "\",\"uptime_ms\":";
  body += millis();
  body += ",\"ota_ready\":";
  body += ready ? "true" : "false";
  body += ",\"updating\":";
  body += updating ? "true" : "false";
  body += "}";
  statusServer.sendHeader("Cache-Control", "no-store");
  statusServer.send(200, "application/json", body);
}
void apiError(int code, const char *message) {
  statusServer.send(code, "application/json", String("{\"error\":\"") + message + "\"}");
}
bool authenticated() {
  statusServer.sendHeader("Cache-Control", "no-store");
  if (statusServer.authenticate("admin", SECRET_OTA_PASSWORD)) return true;
  apiError(401, "Enter your controller password.");
  return false;
}
void sendLightState() {
  char buffer[384];
  writeLightState(buffer, sizeof(buffer));
  statusServer.send(200, "application/json", buffer);
}
void controlLights() {
  if (!authenticated()) return;
  if (statusServer.header("X-Holiday-Control") != "1") {
    apiError(403, "Use the lights control page."); return;
  }
  const String origin = statusServer.header("Origin");
  if (origin.length() && origin != String("http://") + statusServer.hostHeader()
      && origin != String("https://") + statusServer.hostHeader()) {
    apiError(403, "This page is not allowed to control the lights."); return;
  }
  if (updating) { apiError(409, "A firmware update is in progress."); return; }
  const String value = statusServer.arg("value");
  if (!statusServer.hasArg("action") || !value.length() || value.length() > 3) {
    apiError(400, "Invalid light setting."); return;
  }
  uint32_t number = 0;
  for (unsigned i = 0; i < value.length(); ++i) {
    if (value[i] < '0' || value[i] > '9') { apiError(400, "Invalid light setting."); return; }
    number = number * 10 + value[i] - '0';
  }
  if (!applyLightCommand(statusServer.arg("action").c_str(), number, millis())) {
    apiError(400, "Invalid light setting."); return;
  }
  sendLightState();
}

}

void beginNetworkUpdates() {
  Serial.println("Firmware: " HOLIDAY_LIGHTS_VERSION);
  enabled = SECRET_WIFI_SSID[0] && strcmp(SECRET_WIFI_SSID, "REPLACE_ME") != 0
      && strlen(SECRET_OTA_PASSWORD) >= 12;
  if (!enabled) {
    indicator.network(NetworkLight::Offline, millis());
    Serial.println("Wi-Fi/OTA disabled: configure private Wi-Fi and OTA credentials.");
    return;
  }
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.setHostname(hostname);
  WiFi.setAutoReconnect(false); // Retry policy chooses between configured networks.
  ArduinoOTA.setHostname(hostname);
  ArduinoOTA.setPassword(SECRET_OTA_PASSWORD);
  ArduinoOTA.setRebootOnSuccess(false); // Give LED #1 a visible success window.
  ArduinoOTA.setTimeout(10000);
  ArduinoOTA.onStart([]() {
    updating = true;
    lastPercent = 101;
    indicator.update(UpdateLight::Receiving, millis());
    refreshStatus();
    Serial.println("OTA: starting; animation output paused.");
  });
  ArduinoOTA.onProgress([](unsigned progress, unsigned total) {
    refreshStatus(); // Called outside flash writes; sends only changed blink phases.
    const unsigned percent = total ? uint64_t(progress) * 100 / total : 0;
    if (percent / 10 != lastPercent / 10) {
      Serial.printf("OTA: %u%%\n", percent);
      lastPercent = percent;
    }
  });
  ArduinoOTA.onEnd([]() {
    completedAt = millis();
    indicator.update(UpdateLight::Complete, completedAt);
    rebootPending = true;
    refreshStatus();
    Serial.println("OTA: complete; green confirmation, then restarting.");
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("OTA: error %u; lights continue.\n", unsigned(error));
    if (updating) restartAnimation = true;
    updating = false;
    indicator.update(UpdateLight::Failed, millis());
    refreshStatus();
  });
  const char *headers[] = {"Authorization", "Origin", "X-Holiday-Control"};
  statusServer.collectHeaders(headers, 3);
  statusServer.on("/", HTTP_GET, []() {
    statusServer.sendHeader("Cache-Control", "no-store");
    statusServer.sendHeader("X-Content-Type-Options", "nosniff");
    statusServer.sendHeader("Content-Security-Policy", "default-src 'self'; script-src 'unsafe-inline'; style-src 'unsafe-inline'; connect-src 'self'; frame-ancestors 'none'");
    statusServer.send_P(200, "text/html", holidayWebApp);
  });
  statusServer.on("/api/state", HTTP_GET, []() { if (authenticated()) sendLightState(); });
  statusServer.on("/api/control", HTTP_POST, controlLights);
  statusServer.on("/status", HTTP_GET, sendStatus);
}

void serviceNetworkUpdates(uint32_t now) {
  if (!enabled) return;
  if (rebootPending) {
    if (uint32_t(now - completedAt) >= StatusIndicator::confirmationMs) ESP.restart();
    return;
  }
  const bool connected = WiFi.status() == WL_CONNECTED;
  if (!connected && ready) {
    statusServer.stop();
    ArduinoOTA.end();
    ready = false;
    if (updating) {
      restartAnimation = true;
      indicator.update(UpdateLight::Failed, now);
    }
    updating = false;
    Serial.println("Wi-Fi disconnected; reconnecting while lights continue.");
  }
  const int attempt = retry.poll(now, connected);
  if (!connected) indicator.network(retry.searching() ? NetworkLight::Searching : NetworkLight::Offline, now);
  if (attempt >= 0) {
    WiFi.disconnect();
    Serial.printf("Wi-Fi: trying configured network %u.\n", unsigned(attempt + 1));
    WiFi.begin(networks[attempt].ssid, networks[attempt].password);
  }
  if (!connected) return;
  if (!ready) {
    ArduinoOTA.begin();
    statusServer.begin();
    ready = true;
    indicator.network(NetworkLight::Connected, now);
    Serial.print("Wi-Fi connected. IP: ");
    Serial.println(WiFi.localIP());
    Serial.println("OTA ready: holiday-lights.local (port 3232).");
  }
  ArduinoOTA.handle(); // Only an actual update blocks while flash is written.
  if (!updating) statusServer.handleClient();
}

bool networkUpdateBusy() { return updating; }
StatusPixel networkStatusPixel(uint32_t now) { return indicator.pixel(now); }
void setStatusFrameCallback(void (*callback)(uint32_t)) { statusFrameCallback = callback; }
bool takeAnimationRestartRequest() {
  const bool request = restartAnimation;
  restartAnimation = false;
  return request;
}
