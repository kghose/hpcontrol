#include <ESP8266HTTPUpdateServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>

extern "C" {
#include <user_interface.h>
}

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

const char root_page[] = "<form action=\"/head\" method=\"POST\">"
                         "<input type=\"submit\" value=\"SET\">"
                         "</form>";

void handle_root() { http_server.send(200, "text/html", root_page); }

void handle_head() {
  // Just flip the LED for now ...
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  http_server.sendHeader("Location", "/");
  http_server.send(303);
}

void setup(void) {
  pinMode(LED_BUILTIN, OUTPUT);
  wificonfig_wifiOff();
  wificonfig_wifiOn();
  boolean result = WiFi.softAP(AP_NAME, ""); // "" => no password

  IPAddress myIP = WiFi.softAPIP();
  mdns.begin(SERVER_NAME, myIP);

  httpUpdater.setup(&http_server);
  http_server.on("/", HTTP_GET, handle_root);
  http_server.on("/head", HTTP_POST, handle_head);
  http_server.begin();
}

void loop(void) {
  http_server.handleClient();
}
