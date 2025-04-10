#include "gpio_debounce.h"

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(zephyr_gpio_debounced_interrupt, LOG_LEVEL_INF);

static void debounce_handler(struct k_work *w)
{
    struct k_work_delayable *work_delayable = CONTAINER_OF(w, struct k_work_delayable, work);
    gpio_debounce_ctx_t *ctx = CONTAINER_OF(work_delayable, gpio_debounce_ctx_t, debounce_work);

    int val = gpio_pin_get_dt(ctx->gpio_dt);
    if (val < 0) {
        LOG_ERR("Failed to read GPIO");
        return;
    }

    if (val != ctx->last_state) {
        ctx->last_state = val;
        if (val && ctx->rising_edge_callback) {
            ctx->rising_edge_callback();
        }
        if (!val && ctx->falling_edge_callback) {
            ctx->falling_edge_callback();
        }
    }

    gpio_pin_interrupt_configure_dt(ctx->gpio_dt, GPIO_INT_EDGE_BOTH);
}

static void gpio_isr(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    gpio_debounce_ctx_t *ctx = CONTAINER_OF(cb, gpio_debounce_ctx_t, gpio_cb);
    gpio_pin_interrupt_configure_dt(ctx->gpio_dt, GPIO_INT_DISABLE);
    k_work_schedule(&ctx->debounce_work, ctx->debounce_delay_ms);
}

int z_impl_gpio_debounce_init(gpio_debounce_ctx_t *ctx,
                       const struct gpio_dt_spec *gpio_dt,
                       const gpio_flags_t gpio_extra_flags,
                       k_timeout_t debounce_ms,
                       void (*rising_edge_callback)(void),
                       void (*falling_edge_callback)(void)
                    )
{
    int ret;

    if (!device_is_ready(gpio_dt->port)) {
        LOG_ERR("GPIO device not ready");
        return -ENODEV;
    }

    ret = gpio_pin_configure_dt(gpio_dt, gpio_extra_flags);
    if (ret < 0) {
        LOG_ERR("Failed to configure GPIO");
        return ret;
    }

    ctx->gpio_dt = gpio_dt;
    ctx->gpio_extra_flags = gpio_extra_flags;
    ctx->debounce_delay_ms = debounce_ms;
    ctx->rising_edge_callback = rising_edge_callback;
    ctx->falling_edge_callback = falling_edge_callback;
    ctx->last_state = gpio_pin_get_dt(gpio_dt);

    gpio_init_callback(&ctx->gpio_cb, gpio_isr, BIT(gpio_dt->pin));
    gpio_add_callback(gpio_dt->port, &ctx->gpio_cb);

    k_work_init_delayable(&ctx->debounce_work, debounce_handler);

    LOG_INF("GPIO debounce context initialized");

    return gpio_pin_interrupt_configure_dt(gpio_dt, GPIO_INT_EDGE_TO_ACTIVE);
}
