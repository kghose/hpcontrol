/*
 * Extremely simple rolling log that is saved to file and can be accessed via
 * the log page. This design is the equivalent of a singleton object.
 */
#ifndef _LOG_H_
#define _LOG_H_
#include <Arduino.h>

void log_error(String msg);
void log_warning(String msg);
void log_info(String msg);
const String get_log();

#endif _LOG_H_
