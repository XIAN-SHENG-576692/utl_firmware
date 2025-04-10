/*
 * Copyright (c) 2023 Libre Solar Technologies GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>

#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>

#define USER_PATH DT_PATH(zephyr_user)

static const struct gpio_dt_spec gpio_dts[] = {
	// GPIO_DT_SPEC_GET(USER_PATH, p000_gpios),
	// GPIO_DT_SPEC_GET(USER_PATH, p001_gpios),
	GPIO_DT_SPEC_GET(USER_PATH, p002_gpios),
	GPIO_DT_SPEC_GET(USER_PATH, p003_gpios),
	GPIO_DT_SPEC_GET(USER_PATH, p004_gpios),
	GPIO_DT_SPEC_GET(USER_PATH, p005_gpios),
	GPIO_DT_SPEC_GET(USER_PATH, p006_gpios),
	GPIO_DT_SPEC_GET(USER_PATH, p007_gpios),
	GPIO_DT_SPEC_GET(USER_PATH, p008_gpios),
	// GPIO_DT_SPEC_GET(USER_PATH, p009_gpios),
	// GPIO_DT_SPEC_GET(USER_PATH, p010_gpios),
	GPIO_DT_SPEC_GET(USER_PATH, p011_gpios),
	GPIO_DT_SPEC_GET(USER_PATH, p012_gpios),
	GPIO_DT_SPEC_GET(USER_PATH, p013_gpios),
	GPIO_DT_SPEC_GET(USER_PATH, p014_gpios),
	GPIO_DT_SPEC_GET(USER_PATH, p015_gpios),
	GPIO_DT_SPEC_GET(USER_PATH, p016_gpios),
	// GPIO_DT_SPEC_GET(USER_PATH, p017_gpios),
	// GPIO_DT_SPEC_GET(USER_PATH, p018_gpios),
	// GPIO_DT_SPEC_GET(USER_PATH, p019_gpios),
	// GPIO_DT_SPEC_GET(USER_PATH, p020_gpios),
	// GPIO_DT_SPEC_GET(USER_PATH, p021_gpios),
	// GPIO_DT_SPEC_GET(USER_PATH, p022_gpios),
	// GPIO_DT_SPEC_GET(USER_PATH, p023_gpios),
	GPIO_DT_SPEC_GET(USER_PATH, p024_gpios),
	GPIO_DT_SPEC_GET(USER_PATH, p025_gpios),
	GPIO_DT_SPEC_GET(USER_PATH, p026_gpios),
	GPIO_DT_SPEC_GET(USER_PATH, p027_gpios),
	GPIO_DT_SPEC_GET(USER_PATH, p028_gpios),
	GPIO_DT_SPEC_GET(USER_PATH, p029_gpios),
	GPIO_DT_SPEC_GET(USER_PATH, p030_gpios),
	GPIO_DT_SPEC_GET(USER_PATH, p031_gpios),

	GPIO_DT_SPEC_GET(USER_PATH, p100_gpios),
	GPIO_DT_SPEC_GET(USER_PATH, p101_gpios),
	GPIO_DT_SPEC_GET(USER_PATH, p102_gpios),
	GPIO_DT_SPEC_GET(USER_PATH, p103_gpios),
	GPIO_DT_SPEC_GET(USER_PATH, p104_gpios),
	GPIO_DT_SPEC_GET(USER_PATH, p105_gpios),
	GPIO_DT_SPEC_GET(USER_PATH, p106_gpios),
	GPIO_DT_SPEC_GET(USER_PATH, p107_gpios),
	GPIO_DT_SPEC_GET(USER_PATH, p108_gpios),
	GPIO_DT_SPEC_GET(USER_PATH, p109_gpios),
	GPIO_DT_SPEC_GET(USER_PATH, p110_gpios),
	GPIO_DT_SPEC_GET(USER_PATH, p111_gpios),
	GPIO_DT_SPEC_GET(USER_PATH, p112_gpios),
	GPIO_DT_SPEC_GET(USER_PATH, p113_gpios),
	GPIO_DT_SPEC_GET(USER_PATH, p114_gpios),
	GPIO_DT_SPEC_GET(USER_PATH, p115_gpios)

};
#define GPIO_DT_LEN (sizeof(gpio_dts) / sizeof(*gpio_dts))

int main(void)
{
	int err;

	// for(size_t i=0; i<GPIO_DT_LEN; i++) {
	// 	if (!gpio_is_ready_dt(gpio_dts + i)) {
	// 		printf("The load switch pin GPIO port is not ready.\n");
	// 		return 0;
	// 	}
	// }
	for(size_t i=0; i<GPIO_DT_LEN; i++) {
		err = gpio_pin_configure_dt((gpio_dts + i), GPIO_OUTPUT_ACTIVE);
		// err = gpio_pin_configure_dt((gpio_dts + i), GPIO_OUTPUT | GPIO_ACTIVE_LOW);
		if (err != 0) {
			printf("Configuring GPIO pin failed: %d\n", err);
			return 0;
		}
	}
	while(1) {
		for(size_t i=0; i<GPIO_DT_LEN; i++) {
			gpio_pin_toggle_dt((gpio_dts + i));
			// k_sleep(K_MSEC(100));
		}
		k_sleep(K_MSEC(1000));
	}
	return 0;
}
