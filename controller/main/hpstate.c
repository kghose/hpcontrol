#include <stddef.h>
#include <string.h> //for memcpy

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include "hpstate.h"

static const char *TAG = "HP State";
SemaphoreHandle_t system_state_mutex = NULL;
HeatPumpState shared_hp_state;

void initialize_system_state_mutex() {
  system_state_mutex = xSemaphoreCreateMutex();
}

bool get_hp_state(HeatPumpState *hp_state) {
  if (system_state_mutex == NULL) {
    ESP_LOGE(TAG, "Use initialize_system_state_mutex in main thread before "
                  "accessing system state.");
    return false;
  }

  if (xSemaphoreTake(system_state_mutex, portMAX_DELAY) == pdTRUE) {
    memcpy(&shared_hp_state, hp_state, sizeof(HeatPumpState));
    return true;
  } else {
    ESP_LOGW(TAG, "Could not read system state: Timeout trying to get lock.");
    return false;
  }
}

bool set_hp_state(HeatPumpState *hp_state) {
  if (system_state_mutex == NULL) {
    ESP_LOGE(
        TAG,
        "Use initialize_mutex in main thread before accessing system state.");
    return false;
  }

  if (xSemaphoreTake(system_state_mutex, portMAX_DELAY) == pdTRUE) {
    memcpy(hp_state, &shared_hp_state, sizeof(HeatPumpState));
    return true;
  } else {
    ESP_LOGW(TAG, "Could not write system state: Timeout trying to get lock.");
    return false;
  }
}
