#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>

typedef struct {
    const struct gpio_dt_spec *gpio_dt;
    struct gpio_callback gpio_cb;
    gpio_flags_t gpio_extra_flags;
    struct k_work_delayable debounce_work;
    void (*rising_edge_callback)(void);
    void (*falling_edge_callback)(void);
    bool last_state;
    k_timeout_t debounce_delay_ms;
} gpio_debounce_ctx_t;

int z_impl_gpio_debounce_init(gpio_debounce_ctx_t *ctx,
                       const struct gpio_dt_spec *gpio_dt,
                       const gpio_flags_t gpio_flag,
                       k_timeout_t debounce_ms,
                       void (*rising_edge_callback)(void),
                       void (*falling_edge_callback)(void)
                    );

#ifdef __cplusplus
}
#endif
