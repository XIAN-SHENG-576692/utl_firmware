#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>

/**
 * @brief Context structure for GPIO debounce handling.
 */
typedef struct {
    const struct gpio_dt_spec *gpio_dt;      /**< Pointer to GPIO device tree specification. */
    struct gpio_callback gpio_cb;            /**< GPIO callback structure for interrupt handling. */
    gpio_flags_t gpio_extra_flags;           /**< Additional GPIO flags (e.g., pull-up/down). */
    struct k_work_delayable debounce_work;   /**< Zephyr delayable work structure for debounce logic. */
    void (*rising_edge_callback)(void);      /**< Callback function to execute on rising edge detection. */
    void (*falling_edge_callback)(void);     /**< Callback function to execute on falling edge detection. */
    bool last_state;                         /**< Last recorded GPIO state, used for edge detection. */
    k_timeout_t debounce_delay_ms;           /**< Debounce delay time (in milliseconds). */
} gpio_debounce_ctx_t;

/**
 * @brief Initialize GPIO debounce detection.
 *
 * Sets up the GPIO pin with debounce logic, allowing you to specify
 * callbacks for rising and/or falling edges. This prevents bouncing
 * artifacts from triggering callbacks multiple times.
 *
 * @param ctx                  Pointer to debounce context structure.
 * @param gpio_dt              Pointer to GPIO device tree specification.
 * @param gpio_flag            GPIO configuration flags (e.g., GPIO_INPUT).
 * @param debounce_ms          Debounce duration (e.g., K_MSEC(20)).
 * @param rising_edge_callback Function to call on rising edge (can be NULL).
 * @param falling_edge_callback Function to call on falling edge (can be NULL).
 *
 * @return 0 if successful, or a negative error code on failure.
 */
int z_impl_gpio_debounce_init(
    gpio_debounce_ctx_t *ctx,
    const struct gpio_dt_spec *gpio_dt,
    const gpio_flags_t gpio_flag,
    k_timeout_t debounce_ms,
    void (*rising_edge_callback)(void),
    void (*falling_edge_callback)(void)
);

/**
 * @example
 *
 * Device Tree (app.overlay):
 * @code{.dts}
 * / {
 *     zephyr,user {
 *         my-gpios = <&gpio0 10 0>;
 *     };
 * };
 * @endcode
 *
 * Main Program (main.c):
 * @code{.c}
 * #include "gpio_debounce.h"
 *
 * #define ZEPHYR_USER_PATH DT_PATH(zephyr_user)
 *
 * const struct gpio_dt_spec my_gpio_dt = GPIO_DT_SPEC_GET(
 *     ZEPHYR_USER_PATH,
 *     my_gpios
 * );
 *
 * gpio_debounce_ctx_t my_gpio_ctx;
 *
 * static void my_gpio_triggered(void)
 * {
 *     LOG_INF("My GPIO is triggered.");
 * }
 *
 * int main(void)
 * {
 *     int ret;
 *
 *     ret = z_impl_gpio_debounce_init(
 *         &my_gpio_ctx,
 *         &my_gpio_dt,
 *         GPIO_INPUT,
 *         K_MSEC(20),
 *         NULL,
 *         my_gpio_triggered
 *     );
 *     if (ret) {
 *         LOG_ERR("My GPIO device not ready");
 *         return -ENODEV;
 *     }
 *
 *     return ret;
 * }
 * @endcode
 */

#ifdef __cplusplus
}
#endif
