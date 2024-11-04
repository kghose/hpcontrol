/*
 * This is deisgned to compose handlers for our app on top of the
 * http_server the main file sets up. This way we will always
 * have the OTA update page, regardless of if we bork the other parts of the
 * app.
 */
#include "webserver.h"

static ESP8266WebServer *http_server;

const char root_page[] = R"WEBPAGE(
<form action="/head" method="POST">
<input type="submit" value="SET">
</form>)WEBPAGE";

void handle_root() { http_server->send(200, "text/html", root_page); }

void handle_head() {
  // Just flip the LED for now ...
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  http_server->sendHeader("Location", "/");
  http_server->send(303);
}

void setup_webpage_handlers(ESP8266WebServer *http_server_) {
  http_server = http_server_;

  http_server->on("/", HTTP_GET, handle_root);
  http_server->on("/head", HTTP_POST, handle_head);
}
