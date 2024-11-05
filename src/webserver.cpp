/*
 * This is deisgned to compose handlers for our app on top of the
 * http_server the main file sets up. This way we will always
 * have the OTA update page, regardless of if we bork the other parts of the
 * app.
 */
#include "webserver.h"
#include "log.h"

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

void program_head() {
  log_info("Program head");
  // Just flip the LED for now ...
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  http_server->sendHeader("Location", "/");
  http_server->send(303);
}

void show_log() {
  http_server->send(200, "text/html", "<h1>Log</h1>" + get_log());
}

void setup_webpage_handlers(ESP8266WebServer *http_server_) {
  http_server = http_server_;

  http_server->on("/", HTTP_GET, display_settings);
  http_server->on("/program_head", HTTP_POST, program_head);
  http_server->on("/show_log", HTTP_GET, show_log);
}
