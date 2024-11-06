/*
 * Stores ssid and password in plain text on the filesystem.
 *
 * NOTE: I tried using a struct for the ssid + password combo and writing that
 * directly to disk but could not read it back using techniques that normally
 * fork for writing and reading structs. <shrug>
 */

#include <ESP8266WiFi.h>
#include <LittleFS.h>

#include "log.h"
#include "wifi.h"

const uint32_t ROUTER_TIMEOUT_MS = 10000;
const char AP_SSID_PREFIX[] = "mini-split-controller";
const char AP_PASSWORD[] = "mini";

bool load_credentials(String &ssid, String &password) {
  File credentials_file = LittleFS.open("credentials", "r");
  if (credentials_file) {
    ssid = credentials_file.readStringUntil('\n');
    ssid.trim();
    password = credentials_file.readStringUntil('\n');
    password.trim();
    log_info("Read WiFi credentials");
    // DEBUG ONLY!
    log_info("SSID: " + ssid);
    log_info("Password: " + password);
    return true;
  } else {
    log_error("Couldn't open credentials file for reading.");
    return false;
  }
}

bool save_credentials(String ssid, String password) {
  File credentials_file = LittleFS.open("credentials", "w");
  if (credentials_file) {
    credentials_file.println(ssid);
    credentials_file.println(password);
    log_info("Saved WiFi credentials");
    return true;
  } else {
    log_error("Couldn't open credentials file for saving.");
    return false;
  }
}

bool connect_to_router() {
  String ssid, password;
  if (!load_credentials(ssid, password)) {
    return false;
  }
  WiFi.mode(WIFI_STA);
  delay(10);

  WiFi.begin(ssid, password);
  uint64_t start_time = millis();
  while (millis() - start_time < ROUTER_TIMEOUT_MS) {
    delay(500);
    if (WiFi.status() == WL_CONNECTED) {
      log_info("Connected to router.");
      return true;
    }
  }
  log_info("Could not connect to router.");
  return false;
}

void setup_as_ap() {
  WiFi.mode(WIFI_AP);
  delay(10);
  // TODO: add a random suffix to ssid
  WiFi.softAP(AP_SSID_PREFIX, AP_PASSWORD);
  log_info("Established AP");
}

void connect_wifi() {
  if (!connect_to_router()) {
    setup_as_ap();
  }
}

/*
void connect_wifi(String ssid, String password) {
  if (!connect_to_router(ssid, password)) {
    setup_as_ap();
  }
}*/
