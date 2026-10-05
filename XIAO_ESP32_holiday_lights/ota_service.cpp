#include <Arduino.h>
#include <ArduinoOTA.h>
#include <WiFi.h>
#include <WebServer.h>
#include "version.h"
#include "wifi_retry.h"
#include "ota_service.h"
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
WebServer statusServer(80); // Read-only version/status; no browser upload endpoint yet.
bool enabled = false, ready = false, updating = false, restartAnimation = false;
unsigned lastPercent = 101;

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
}

void beginNetworkUpdates() {
  Serial.println("Firmware: " HOLIDAY_LIGHTS_VERSION);
  enabled = SECRET_WIFI_SSID[0] && strcmp(SECRET_WIFI_SSID, "REPLACE_ME") != 0
      && strlen(SECRET_OTA_PASSWORD) >= 12;
  if (!enabled) {
    Serial.println("Wi-Fi/OTA disabled: configure private Wi-Fi and OTA credentials.");
    return;
  }
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.setHostname(hostname);
  WiFi.setAutoReconnect(false); // Retry policy chooses between configured networks.
  ArduinoOTA.setHostname(hostname);
  ArduinoOTA.setPassword(SECRET_OTA_PASSWORD);
  ArduinoOTA.setRebootOnSuccess(true);
  ArduinoOTA.setTimeout(10000);
  ArduinoOTA.onStart([]() {
    updating = true;
    lastPercent = 101;
    Serial.println("OTA: starting; animation output paused.");
  });
  ArduinoOTA.onProgress([](unsigned progress, unsigned total) {
    const unsigned percent = total ? uint64_t(progress) * 100 / total : 0;
    if (percent / 10 != lastPercent / 10) {
      Serial.printf("OTA: %u%%\n", percent);
      lastPercent = percent;
    }
  });
  ArduinoOTA.onEnd([]() { Serial.println("OTA: complete; restarting."); });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("OTA: error %u; lights continue.\n", unsigned(error));
    if (updating) restartAnimation = true;
    updating = false;
  });
  statusServer.on("/", HTTP_GET, sendStatus);
  statusServer.on("/status", HTTP_GET, sendStatus);
}

void serviceNetworkUpdates(uint32_t now) {
  if (!enabled) return;
  const bool connected = WiFi.status() == WL_CONNECTED;
  if (!connected && ready) {
    statusServer.stop();
    ArduinoOTA.end();
    ready = false;
    if (updating) restartAnimation = true;
    updating = false;
    Serial.println("Wi-Fi disconnected; reconnecting while lights continue.");
  }
  const int attempt = retry.poll(now, connected);
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
    Serial.print("Wi-Fi connected. IP: ");
    Serial.println(WiFi.localIP());
    Serial.println("OTA ready: holiday-lights.local (port 3232).");
  }
  ArduinoOTA.handle(); // Only an actual update blocks while flash is written.
  if (!updating) statusServer.handleClient();
}

bool networkUpdateBusy() { return updating; }
bool takeAnimationRestartRequest() {
  const bool request = restartAnimation;
  restartAnimation = false;
  return request;
}
