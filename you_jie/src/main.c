#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/device.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/drivers/gpio.h>
#include <math.h>

static const struct pwm_dt_spec custompwm[] = 
{
	PWM_DT_SPEC_GET_BY_IDX(DT_ALIAS(mycustompwm), 0),
	PWM_DT_SPEC_GET_BY_IDX(DT_ALIAS(mycustompwm), 1),
	// PWM_DT_SPEC_GET_BY_IDX(DT_ALIAS(mycustompwm), 2),
	// PWM_DT_SPEC_GET_BY_IDX(DT_ALIAS(mycustompwm), 3),
};

#define PWM_DEVICES_LENGTH sizeof(custompwm) / sizeof(custompwm[0])

static const struct gpio_dt_spec gpio_devices[] = {
	GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios),
	GPIO_DT_SPEC_GET(DT_ALIAS(led1), gpios),
	GPIO_DT_SPEC_GET(DT_ALIAS(led2), gpios),
	GPIO_DT_SPEC_GET(DT_ALIAS(led3), gpios),
};

#define GPIO_DEVICES_LENGTH sizeof(gpio_devices) / sizeof(gpio_devices[0])

int main(void)
{
	int error;

	for(size_t i=0; i<PWM_DEVICES_LENGTH; i++)
	{
		if (!device_is_ready(custompwm[i].dev)) {
			printk("Error: PWM device %s is not ready\n", custompwm[i].dev->name);
			return 0;
		}
	}

	for(size_t i=0; i<GPIO_DEVICES_LENGTH; i++)
	{
		if (!gpio_pin_configure_dt(&(gpio_devices[i]), GPIO_OUTPUT_ACTIVE)) {
			// printk("Error: GPIO device %s is not ready\n", gpio_devices[i]);
			return 0;
		}
	}

	error = pwm_set_pulse_dt(
		custompwm + 0, 
		custompwm->period * 1.0
	);
	error = gpio_pin_set_dt(&(gpio_devices[0]), true);
	error = gpio_pin_set_dt(&(gpio_devices[1]), false);
	error = gpio_pin_set_dt(&(gpio_devices[2]), true);
	error = gpio_pin_set_dt(&(gpio_devices[3]), false);

	while (1);

	return 0;
}
