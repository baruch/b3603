#include <stdint.h>

#include "config.h"

void pwm_init(void);
void output_commit(cfg_output_t *cfg, cfg_system_t *sys);
void output_check_state(cfg_output_t *cfg, uint8_t state_constant_current);
