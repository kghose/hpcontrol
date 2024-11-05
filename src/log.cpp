/*
 * Very simple log
 */
#include "log.h"
#include <LittleFS.h>

const char LOG_FILE[] = "hplog.txt";
const uint32_t MAX_SIZE_BYTES = 2000; // If the file is larger, we'll rewind and
                                      // overwrite the oldest entries.
static File log_file;

void log_msg(String msg) {
  if (!log_file) {
    LittleFS.begin();
    log_file = LittleFS.open(LOG_FILE, "a+");
  }
  if (!log_file) {
    return; // Don't crash the application
  }
  log_file.println(String(time(nullptr)) + ": " + msg + "\n");
  if (log_file.position() > MAX_SIZE_BYTES) {
    log_file.seek(0, SeekSet);
  }
}

void log_error(String msg) { log_msg(String("[ERROR] ") + msg); }

void log_warning(String msg) { log_msg(String("[WARN] ") + msg); }

void log_info(String msg) { log_msg(String("[INFO] ") + msg); }

const String get_log() {
  uint64_t cur_pos = log_file.position();
  log_file.seek(0, SeekSet);
  String log_text = log_file.readString();
  log_file.seek(cur_pos, SeekSet);
  return log_text;
}
