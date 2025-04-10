#include "bluetooth_config.h"

#include <zephyr/settings/settings.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/hci.h>

#include "on_receive.h"
#include "bluetooth_config_lock.h"

/* Define advertising data */
static const struct bt_data ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR),
    BT_DATA_BYTES(BT_DATA_UUID16_ALL, 0x0F, 0x18) // Service UUID
};

static struct bt_conn *current_conn = NULL;  // Store current connection

/* Callback: when device is connected */
static void connected(struct bt_conn *conn, uint8_t err)
{
    if (err) {
        printk("Connection failed (err %d)\n", err);
        return;
    }

    printk("Device connected\n");
    current_conn = bt_conn_ref(conn);  // Store the connection

    /* Stop advertising after connection */
    int ret = bt_le_adv_stop();
    if (ret) {
        printk("Failed to stop advertising (err %d)\n", ret);
    } else {
        printk("Advertising stopped, device is no longer discoverable\n");
    }
}

/* Callback: when device is disconnected */
static void disconnected(struct bt_conn *conn, uint8_t reason)
{
    printk("Device disconnected (reason %d)\n", reason);
    
    if (current_conn) {
        bt_conn_unref(current_conn);
        current_conn = NULL;
    }

    /* Restart advertising after disconnection */
    int ret = bt_le_adv_start(BT_LE_ADV_CONN_NAME, ad, ARRAY_SIZE(ad), NULL, 0);
    if (ret) {
        printk("Failed to restart advertising (err %d)\n", ret);
    } else {
        printk("Advertising restarted, device is discoverable\n");
    }
}

static void on_ccc_cfg_changed_1(const struct bt_gatt_attr *attr, uint16_t value) {
    if (value == BT_GATT_CCC_NOTIFY) {
        printk("Notify enabled for Characteristic 1\n");
    } else {
        printk("Notify disabled for Characteristic 1\n");
    }
}

static void on_ccc_cfg_changed_2(const struct bt_gatt_attr *attr, uint16_t value) {
    if (value == BT_GATT_CCC_NOTIFY) {
        printk("Notify enabled for Characteristic 2\n");
    } else {
        printk("Notify disabled for Characteristic 2\n");
    }
}

/* GATT service definition */
BT_GATT_SERVICE_DEFINE(zephyr_bluetooth_example_service,
    BT_GATT_PRIMARY_SERVICE(BT_UUID_DECLARE_16(0x180F)),

    // Write characteristic
    BT_GATT_CHARACTERISTIC(BT_UUID_DECLARE_16(0x2A19),
                           BT_GATT_CHRC_WRITE,
                           BT_GATT_PERM_WRITE,
                           NULL, on_receive, NULL),

    // Notify characteristic 1
    BT_GATT_CHARACTERISTIC(BT_UUID_DECLARE_16(0x2A1A),
                           BT_GATT_CHRC_NOTIFY,
                           BT_GATT_PERM_NONE,
                           NULL, NULL, NULL),
    BT_GATT_CCC(on_ccc_cfg_changed_1, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),

    // Notify characteristic 2
    BT_GATT_CHARACTERISTIC(BT_UUID_DECLARE_16(0x2A1B),
                           BT_GATT_CHRC_NOTIFY,
                           BT_GATT_PERM_NONE,
                           NULL, NULL, NULL),
    BT_GATT_CCC(on_ccc_cfg_changed_2, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
);


/* Bluetooth callback functions */
static struct bt_conn_cb conn_callbacks = {
    .connected = connected,
    .disconnected = disconnected,
};

/* Main function */
int bluetooth_config(void)
{
    int err;

    bluetooth_config_lock_init();

    printk("Initializing Bluetooth...\n");

    err = bt_enable(NULL);
    if (err) {
        printk("Bluetooth initialization failed (err %d)\n", err);
        return err;
    }

    printk("Bluetooth started successfully\n");

    bt_conn_cb_register(&conn_callbacks);

    if (IS_ENABLED(CONFIG_SETTINGS)) {
        settings_load();
    }

    /* Start advertising */
    err = bt_le_adv_start(BT_LE_ADV_CONN_NAME, ad, ARRAY_SIZE(ad), NULL, 0);
    if (err) {
        printk("Failed to start advertising (err %d)\n", err);
        return err;
    }

    printk("Advertising started, device is discoverable...\n");

    bluetooth_config_lock_set_ready();

    return 0;
}

void bluetooth_get_bt_gatt_service_static(
    const struct bt_gatt_service_static **services
)
{
    *services = &zephyr_bluetooth_example_service;
    return;
}
