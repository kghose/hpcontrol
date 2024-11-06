/*
 * This is deisgned to compose handlers for our app on top of the
 * http_server the main file sets up. This way we will always
 * have the OTA update page, regardless of if we bork the other parts of the
 * app.
 */
#include "webserver.h"
#include "log.h"
#include "wifi.h"

static ESP8266WebServer *http_server;

const char root_page[] = R"WEBPAGE(
<a href="/show_log">Log</a>
</hr>
<form action="/program_head" method="POST">
<input type="submit" value="SET">
</form>
)WEBPAGE";

void display_settings() {
  http_server->send(200, "text/html",
                    (digitalRead(LED_BUILTIN) ? String("ON") : String("OFF")) +
                        String(root_page));
}

void show_wifi_credentials() {
  const char page[] = R"WEBPAGE(
<form action="/set_wifi" method="POST">
<label for="ssid">SSID</label><input type="text" name="ssid"><br/>
<label for="password">Password</label><input type="text" name="password"><br/>
<input type="submit" value="Save">
</form>
)WEBPAGE";
  http_server->send(200, "text/html", page);
}

void update_wifi_credentials() {
  log_info("Updating WiFi credentials");
  save_credentials(http_server->arg("ssid"), http_server->arg("password"));
  connect_wifi();
  // connect_wifi(http_server->arg("ssid"), http_server->arg("password"));
  http_server->sendHeader("Location", "/");
  http_server->send(303);
}

void program_head() {
  log_info("Program head");
  // Just flip the LED for now ...
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  http_server->sendHeader("Location", "/");
  http_server->send(303);
}

void show_log() {
  http_server->send(200, "text/html",
                    "<h1>Log</h1><pre>" + get_log() + "</pre>");
}

void setup_webpage_handlers(ESP8266WebServer *http_server_) {
  http_server = http_server_;

  http_server->on("/", HTTP_GET, display_settings);
  http_server->on("/wifi", HTTP_GET, show_wifi_credentials);
  http_server->on("/set_wifi", HTTP_POST, update_wifi_credentials);
  http_server->on("/program_head", HTTP_POST, program_head);
  http_server->on("/show_log", HTTP_GET, show_log);
}
