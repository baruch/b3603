#include "outputs.h"

#include "stm8s.h"

#define PWM_VAL 0x2000
#define PWM_HIGH (PWM_VAL >> 8)
#define PWM_LOW (PWM_VAL & 0xFF)

void pwm_init(void)
{
	/* Timer 1 Channel 1 for Iout control */
	TIM1_CR1 = 0x10; // Down direction
	TIM1_ARRH = PWM_HIGH; // Reload counter = 16384
	TIM1_ARRL = PWM_LOW;
	TIM1_PSCRH = 0; // Prescaler 0 means division by 1
	TIM1_PSCRL = 0;
	TIM1_RCR = 0; // Continuous

	TIM1_CCMR1 = 0x70;    //  Set up to use PWM mode 2.
	TIM1_CCER1 = 0x03;    //  Output is enabled for channel 1, active low
	TIM1_CCR1H = 0x00;      //  Start with the PWM signal off
	TIM1_CCR1L = 0x00;

	TIM1_BKR = 0x80;       //  Enable the main output.

	/* Timer 2 Channel 1 for Vout control */
	TIM2_ARRH = PWM_HIGH; // Reload counter = 16384
	TIM2_ARRL = PWM_LOW;
	TIM2_PSCR = 0; // Prescaler 0 means division by 1
	TIM2_CR1 = 0x00;

	TIM2_CCMR1 = 0x70;    //  Set up to use PWM mode 2.
	TIM2_CCER1 = 0x03;    //  Output is enabled for channel 1, active low
	TIM2_CCR1H = 0x00;      //  Start with the PWM signal off
	TIM2_CCR1L = 0x00;

	// Timers are still off, will be turned on when output is turned on
}

void cvcc_led_cc(void)
{
	PA_ODR |= (1<<3);
	PA_DDR |= (1<<3);
}

void cvcc_led_cv(void)
{
	PA_ODR &= ~(1<<3);
	PA_DDR |= (1<<3);
}

void cvcc_led_off(void)
{
	PA_DDR &= ~(1<<3);
}

uint8_t output_state(void)
{
	return (PB_ODR & (1<<4)) ? 0 : 1;
}

void control_voltage(cfg_output_t *cfg)
{
	uint32_t tmp;
	uint16_t ctr;

	// tmp = cfg_vset * 0.073
	tmp = cfg->vset;
	tmp *= 74752; // (73<<10)
	tmp /= 1000;
	tmp >>= 10;

	// tmp += 0.033
	tmp += 34; //(33<<10)/1000;

	// Here we have in tmp the Vout control value
	// Now we want to calculate the PWM counter
	// (tmp/3.3) * PWM_VAL

	// tmp *= PWM_VAL
	tmp *= PWM_VAL;
	tmp >>= 10; // Now we strip the fraction from the number

	// tmp /= 3.3v => tmp *= (1/3.3v) 
	tmp *= 310; //(303<<10)/1000
	tmp >>= 10;

	ctr = tmp;

	TIM2_CCR1H = ctr >> 8;
	TIM2_CCR1L = ctr & 0xFF;
	TIM2_CR1 |= 0x01; // Enable timer
}

void control_current(cfg_output_t *cfg)
{
	uint32_t tmp;
	uint16_t ctr;

	tmp = cfg->cset;
	tmp *= 819; //(8<<10) / 10;
	tmp >>= 10;

	tmp += 164; //(16<<10) / 100;

	tmp *= PWM_VAL;
	tmp >>= 10;

	tmp *= 310; // 1/3.3
	tmp >>= 10;

	ctr = tmp;

	TIM1_CCR1H = ctr >> 8;
	TIM1_CCR1L = ctr & 0xFF;
	TIM1_CR1 |= 0x01; // Enable timer
}

void output_commit(cfg_output_t *cfg)
{
	if (cfg->output) {
		control_voltage(cfg);
		control_current(cfg);
	}

	if (cfg->output != output_state()) {
		// Startup and shutdown orders need to be in reverse order
		if (cfg->output) {
			// We turned on the PWMs above already
			PB_ODR &= ~(1<<4);

			// Set the CV/CC to some value, it will get reset to actual on next polling cycle
			cvcc_led_cv();
		} else {
			// Set Output Enable OFF
			PB_ODR |= (1<<4);

			// Turn off PWM for Iout
			TIM1_CCR1H = 0;
			TIM1_CCR1L = 0;
			TIM1_CR1 &= 0xFE; // Disable timer

			// Turn off PWM for Vout
			TIM2_CCR1H = 0;
			TIM2_CCR1L = 0;
			TIM2_CR1 &= 0xFE; // Disable timer

			// Turn off CV/CC led
			cvcc_led_off();
		}
	}
}

void output_check_state(cfg_output_t *cfg, uint8_t state_constant_current)
{
	if (cfg->output) {
		if (state_constant_current)
			cvcc_led_cc();
		else
			cvcc_led_cv();
	}
}
