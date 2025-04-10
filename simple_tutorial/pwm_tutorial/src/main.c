#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/device.h>
#include <zephyr/drivers/pwm.h>
#include <math.h>

static const struct pwm_dt_spec custompwm[] = 
{
	PWM_DT_SPEC_GET_BY_IDX(DT_ALIAS(mycustompwm), 0),
	PWM_DT_SPEC_GET_BY_IDX(DT_ALIAS(mycustompwm), 1),
	// PWM_DT_SPEC_GET_BY_IDX(DT_ALIAS(mycustompwm), 2),
	// PWM_DT_SPEC_GET_BY_IDX(DT_ALIAS(mycustompwm), 3),
};

#define PWM_LENGTH sizeof(custompwm) / sizeof(custompwm[0])

#define PWM_FREQ 100 // Hz
#define CYCLE_FREQ 1 // Hz

#define STEPS 8
float levels[STEPS]; 

#define TWO_PI 6.28318530718f

int main(void)
{
	int error;

	for(size_t i=0; i<PWM_LENGTH; i++)
	{
		if (!device_is_ready(custompwm[i].dev)) {
			printk("Error: PWM device %s is not ready\n", custompwm[i].dev->name);
			return 0;
		}
	}

	for(size_t i=0; i<PWM_LENGTH; i++)
	{
		error = pwm_set_pulse_dt(
			custompwm + i, 
			custompwm->period * 0.6
		);
		if (error) {
			printk("Error %d: %s failed to execute pwm_set_pulse_dt.\n", error, custompwm[i].dev->name);
			return 0;
		}
	}

	k_sleep(K_SECONDS(1));

	for(size_t i=0; i<PWM_LENGTH; i++)
	{
		error = pwm_set_pulse_dt(
			custompwm + i, 
			0
		);
		if (error) {
			printk("Error %d: %s failed to execute pwm_set_pulse_dt.\n", error, custompwm[i].dev->name);
			return 0;
		}
	}

	k_sleep(K_SECONDS(1));

	for(size_t i=0; i<PWM_LENGTH; i++)
	{
		error = pwm_set_dt(
			custompwm + i, 
			PWM_HZ(PWM_FREQ), 
			(uint32_t) (PWM_HZ(PWM_FREQ) * 0.1)
		);
		if (error) {
			printk("Error %d: %s failed to execute pwm_set_dt.\n", error, custompwm[i].dev->name);
			return 0;
		}
	}

	k_sleep(K_SECONDS(1));

	for (int i =0; i < STEPS; i++) 
	{
		levels[i] = 0.5f + 0.45f * sin(TWO_PI * i / STEPS);
	}

	int count = 0;

	while (1) 
	{
		for(size_t i=0; i<PWM_LENGTH; i++)
		{
			error = pwm_set_dt(
				&custompwm[i], 
				PWM_HZ(PWM_FREQ), 
				(uint32_t) PWM_HZ(PWM_FREQ)*levels[count++ % STEPS]
			);
			if (error) {
				printk("Error %d: failed to set pulse width\n", error);
				return 0;
			}
		}
		k_sleep(K_USEC(1000000U/(CYCLE_FREQ*STEPS)));
	}

	return 0;
}
