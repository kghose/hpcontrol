#include <stdbool.h>
#include <stdint.h>

typedef struct {
  char name[15];  
  bool on;
  int32_t set_temp_c;
} HeadState;

typedef struct {
  HeadState head_state[4];
} HeatPumpState;


void initialize_system_state_mutex();
bool get_hp_state(HeatPumpState *hp_state);
bool set_hp_state(HeatPumpState *hp_state);
