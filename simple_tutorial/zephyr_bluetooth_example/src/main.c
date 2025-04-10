#include "bluetooth_config.h"

#include <zephyr/bluetooth/hci.h>

/* Main function */
int main(void)
{
    int err;

    err = bluetooth_config();
    if (err) {
        printk("Failed to bluetooth_config (err %d)\n", err);
        return err;
    }

    return 0;
}
