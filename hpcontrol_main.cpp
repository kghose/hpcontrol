#include <ESP8266HTTPUpdateServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <LittleFS.h>

extern "C" {
#include <user_interface.h>
}

// It has to be this relative path otherwise the Arduino precompiler
// does something funny causing gcc to report duplicate definition errors.
#include "src/log.h"
#include "src/webserver.h"
#include "src/wifi.h"

const char AP_NAME[] = "grue_ap";
const char SERVER_NAME[] = "grue";
const char SERVICE_TYPE[] = "despicable-hp";

MDNSResponder mdns;
ESP8266WebServer http_server(80);
ESP8266HTTPUpdateServer httpUpdater;


void setup(void) {

  LittleFS.begin(); // Initialize the FS for all other modules to use

  pinMode(LED_BUILTIN, OUTPUT);

  connect_wifi();
  IPAddress myIP = WiFi.localIP();

  mdns.begin(SERVER_NAME, myIP);
  mdns.addService(SERVICE_TYPE, "tcp", 80);

  httpUpdater.setup(&http_server); // we'll always have the OTA updater at least
  setup_webpage_handlers(&http_server);
  http_server.begin();
}

void loop(void) {
  http_server.handleClient();
  mdns.update();
}
