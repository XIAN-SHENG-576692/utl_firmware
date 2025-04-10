#include "ad5940_port_gpio.h"

#include "driver_gpio_impl_zephyr.h"

const struct gpio_dt_spec ad5940_gpio7 = GPIO_DT_SPEC_GET(
    ZEPHYR_USER_PATH,
    ad5940_gpio7_gpios
);

gpio_debounce_ctx_t ad5940_gpio7_ctx;
