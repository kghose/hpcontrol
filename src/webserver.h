/*
 * For notes on this design please see the implementation file
 * webserver.cpp
 */
#ifndef _WEBSERVER_H_
#define _WEBSERVER_H_

#include <ESP8266WebServer.h>

void setup_webpage_handlers(ESP8266WebServer *http_server_);

#endif // _WEBSERVER_H_
