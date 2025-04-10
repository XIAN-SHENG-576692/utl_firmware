#include "ble.h"

#include <zephyr/settings/settings.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/hci.h>

/* 定義廣播數據 */
static const struct bt_data ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR),
    BT_DATA_BYTES(BT_DATA_UUID16_ALL, 0x0F, 0x18) // 服務 UUID
};

static struct bt_conn *current_conn = NULL;  // 存儲當前的連線

/* 回呼函式：當裝置連線 */
static void connected(struct bt_conn *conn, uint8_t err)
{
    if (err) {
        printk("連線失敗 (err %d)\n", err);
        return;
    }

    printk("裝置已連線\n");
    current_conn = bt_conn_ref(conn);  // 存儲連線

    /* 連線後停止廣播 */
    int ret = bt_le_adv_stop();
    if (ret) {
        printk("停止廣播失敗 (err %d)\n", ret);
    } else {
        printk("已停止廣播，裝置不再被發現\n");
    }
}

/* 回呼函式：當裝置斷線 */
static void disconnected(struct bt_conn *conn, uint8_t reason)
{
    printk("裝置已斷線 (reason %d)\n", reason);
    
    if (current_conn) {
        bt_conn_unref(current_conn);
        current_conn = NULL;
    }

    /* 斷線後恢復廣播 */
    int ret = bt_le_adv_start(BT_LE_ADV_CONN_NAME, ad, ARRAY_SIZE(ad), NULL, 0);
    if (ret) {
        printk("恢復廣播失敗 (err %d)\n", ret);
    } else {
        printk("已恢復廣播，裝置可被掃描\n");
    }
}

/* GATT 服務 - 資料處理 */
#define MAX_DATA_LEN 20  // 定義最大封包長度
static uint8_t rx_buffer[MAX_DATA_LEN];

/* 當手機寫入特徵值時 */
static ssize_t on_receive(struct bt_conn *conn, 
                          const struct bt_gatt_attr *attrs,
                          const void *buf, uint16_t len, uint16_t offset, uint8_t flags)
{
    if (len > MAX_DATA_LEN) {
        len = MAX_DATA_LEN;  // 確保不超過最大長度
    }

    memcpy(rx_buffer, buf, len);  // 複製收到的資料

    printk("收到資料: ");
    for (int i = 0; i < len; i++) {
        printk("%02X ", rx_buffer[i]);
        rx_buffer[i] += 1;  // 每個 byte +1
    }
    printk("\n");

    /* 回傳處理後的資料 */
    int err = bt_gatt_notify(NULL, &attrs[2], rx_buffer, len);
    if (err) {
        printk("回傳資料失敗 (err %d)\n", err);
    } else {
        printk("已回傳處理後的資料\n");
    }

    return len;
}

/* GATT 服務定義 */
BT_GATT_SERVICE_DEFINE(test_service,
    BT_GATT_PRIMARY_SERVICE(BT_UUID_DECLARE_16(0x180F)),  // UUID 可自訂
    BT_GATT_CHARACTERISTIC(BT_UUID_DECLARE_16(0x2A19),
                           BT_GATT_CHRC_WRITE,
                           BT_GATT_PERM_WRITE,
                           NULL, on_receive, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_DECLARE_16(0x2A1A),
                           BT_GATT_CHRC_NOTIFY,
                           BT_GATT_PERM_NONE,
                           NULL, NULL, NULL),
    BT_GATT_CCC(NULL, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
);

/* 藍牙回呼函式 */
static struct bt_conn_cb conn_callbacks = {
    .connected = connected,
    .disconnected = disconnected,
};

/* 主函式 */
int BLE_init(void)
{
    int err;

    printk("初始化藍牙...\n");

    err = bt_enable(NULL);
    if (err) {
        printk("藍牙初始化失敗 (err %d)\n", err);
        return err;
    }

    printk("藍牙啟動成功\n");

    bt_conn_cb_register(&conn_callbacks);

    if (IS_ENABLED(CONFIG_SETTINGS)) {
		settings_load();
	}

    /* 啟動廣播 */
    err = bt_le_adv_start(BT_LE_ADV_CONN_NAME, ad, ARRAY_SIZE(ad), NULL, 0);
    if (err) {
        printk("廣播啟動失敗 (err %d)\n", err);
        return err;
    }

    printk("廣播中，裝置可被掃描...\n");

	return 0;
}
