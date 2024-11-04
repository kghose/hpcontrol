#include <ESP8266HTTPUpdateServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>

extern "C" {
#include <user_interface.h>
}

// It has to be this relative path otherwise the Arduino precompiler
// does something funny causing gcc to report duplicate definition errors.
#include "src/webserver.h"

const char AP_NAME[] = "grue_ap";
const char SERVER_NAME[] = "grue";

MDNSResponder mdns;
ESP8266WebServer http_server(80);
ESP8266HTTPUpdateServer httpUpdater;

void wificonfig_wifiOn() {
  wifi_fpm_do_wakeup();
  wifi_fpm_close();
  delay(100);
}

#define FPM_SLEEP_MAX_TIME 0xFFFFFFF

void wificonfig_wifiOff() {
  wifi_station_disconnect();
  wifi_set_opmode(NULL_MODE);
  wifi_set_sleep_type(MODEM_SLEEP_T);
  wifi_fpm_open();
  wifi_fpm_do_sleep(FPM_SLEEP_MAX_TIME);
  delay(100);
}

void setup(void) {
  pinMode(LED_BUILTIN, OUTPUT);
  wificonfig_wifiOff();
  wificonfig_wifiOn();
  boolean result = WiFi.softAP(AP_NAME, ""); // "" => no password

  IPAddress myIP = WiFi.softAPIP();
  mdns.begin(SERVER_NAME, myIP);

  httpUpdater.setup(&http_server); // we'll always have the OTA updater at least
  setup_webpage_handlers(&http_server);
  http_server.begin();
}

void loop(void) { http_server.handleClient(); }
