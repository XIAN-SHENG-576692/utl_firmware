#include "on_receive.h"

/* GATT service - data handler */
#define MAX_DATA_LEN 20  // Define maximum packet length
static uint8_t rx_buffer[MAX_DATA_LEN];

/* When phone writes to characteristic */
ssize_t on_receive(
    struct bt_conn *conn, 
    const struct bt_gatt_attr *attr,
    const void *buf, 
    uint16_t len, 
    uint16_t offset, 
    uint8_t flags
)
{
    if (len > MAX_DATA_LEN) {
        len = MAX_DATA_LEN;  // Ensure not to exceed max length
    }

    memcpy(rx_buffer, buf, len);  // Copy received data

    printk("Received data: ");
    for (int i = 0; i < len; i++) {
        printk("%02X ", rx_buffer[i]);
        rx_buffer[i] += 1;  // Increment each byte by 1
    }
    printk("\n");

    /* 
     * Send back the processed data
     *
     * Note:
     * - `attr` refers to the attribute for which we just received data.
     * - According to the GATT service definition, the second following attribute 
     *   is 0x2A1A — the notification attribute.
     */
    int err = bt_gatt_notify(NULL, &attr[2], rx_buffer, len);
    if (err) {
        printk("Failed to send notification (err %d)\n", err);
    } else {
        printk("Processed data sent\n");
    }

    return len;
}
