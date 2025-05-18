#include "ad5940_port_intc0_impl_zephyr.h"

#include "gpio_debounce.h"
#include "ad5940_port_gpio_impl_zephyr.h"

#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(ad5940_intc0, LOG_LEVEL_INF);

int AD5940_intc0_impl_zephyr_init(void (*callback)(void))
{
    int ret;

    ret = z_impl_gpio_debounce_init(
        &ad5940_gpio7_ctx,
        &ad5940_gpio7,
        GPIO_INPUT,
        K_MSEC(20),
        NULL,
        callback
    );
    if (ret) {
        LOG_ERR("GPIO device not ready");
        return -ENODEV;
    }

    return ret;
}
