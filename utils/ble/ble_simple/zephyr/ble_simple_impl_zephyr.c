#include "ble_simple_impl_zephyr.h"

#include <stdint.h>
#include <stdbool.h>

#include <zephyr/settings/settings.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/hci.h>

#include <bluetooth/services/nus.h>

#include <zephyr/logging/log.h>
#define LOG_MODULE_NAME ble_simple_impl
LOG_MODULE_REGISTER(LOG_MODULE_NAME);

#define DEVICE_NAME CONFIG_BT_DEVICE_NAME
#define DEVICE_NAME_LENGTH (sizeof(DEVICE_NAME) - 1)

static struct bt_conn *current_conn;
static struct bt_conn *auth_conn;

static const struct bt_data ad[] = {
	BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
	BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LENGTH),
};

static const struct bt_data sd[] = {
	BT_DATA_BYTES(BT_DATA_UUID128_ALL, BT_UUID_NUS_VAL),
};

static volatile bool _is_inited = false;
static K_MUTEX_DEFINE(_init_mutex);
static K_CONDVAR_DEFINE(_init_condvar);

static volatile bool _is_connected = false;
static K_MUTEX_DEFINE(_connection_mutex);
static K_CONDVAR_DEFINE(_connection_condvar);

static K_MUTEX_DEFINE(_wait_new_packet_received_mutex);
static K_CONDVAR_DEFINE(_wait_new_packet_received_condvar);

/*MTU exchange*/
static void mtu_exchange_cb(
	struct bt_conn *conn, 
	uint8_t err,
	struct bt_gatt_exchange_params *params
)
{
    LOG_INF("MTU exchange %u %s (%u)", 
		bt_conn_index(conn),
		err == 0U ? "successful" : "failed", 
		bt_gatt_get_mtu(conn)
	);
	return;
}

static uint8_t request_mtu_exchange(struct bt_conn *conn)
{	int err;
	static struct bt_gatt_exchange_params exchange_params;
	exchange_params.func = mtu_exchange_cb;

	err = bt_gatt_exchange_mtu(current_conn, &exchange_params);
	if (err) {
		LOG_WRN("MTU exchange failed (err %d)", err);
	} else {
		LOG_INF("MTU exchange pending");
	}
	return err;
}

static uint8_t request_data_len_update(struct bt_conn *conn)
{
	int err;
	err = bt_conn_le_data_len_update(conn, BT_LE_DATA_LEN_PARAM_MAX);
		if (err) {
			LOG_ERR("LE data length update request failed: %d",  err);
		}
	return err;
}

static uint8_t request_phy_update(struct bt_conn *conn)
{
	int err;
	err = bt_conn_le_phy_update(conn, BT_CONN_LE_PHY_PARAM_2M);
		if (err) {
			LOG_ERR("Phy update request failed: %d",  err);
		}
	return err;
}

static void connected(struct bt_conn *conn, uint8_t err)
{
	char addr[BT_ADDR_LE_STR_LEN];

	if (err) {
		LOG_ERR("Connection failed (err %u)", err);
		return;
	}

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
	LOG_INF("Connected %s", addr);

	current_conn = bt_conn_ref(conn);

	request_mtu_exchange(conn);
	request_data_len_update(conn);
	request_phy_update(conn);

    // k_mutex_lock(&_connection_mutex, K_FOREVER);
    _is_connected = true;
    k_condvar_broadcast(&_connection_condvar);
    // k_mutex_unlock(&_connection_mutex);

	return;
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	LOG_INF("Disconnected: %s (reason %u)", addr, reason);

	if (auth_conn) {
		bt_conn_unref(auth_conn);
		auth_conn = NULL;
	}

	if (current_conn) {
		bt_conn_unref(current_conn);
		current_conn = NULL;
	}

    // k_mutex_lock(&_connection_mutex, K_FOREVER);
    _is_connected = false;
    k_condvar_broadcast(&_connection_condvar);
    // k_mutex_unlock(&_connection_mutex);

	return;
}

#ifdef CONFIG_BT_NUS_SECURITY_ENABLED
static void security_changed(struct bt_conn *conn, bt_security_t level,
			     enum bt_security_err err)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	if (!err) {
		LOG_INF("Security changed: %s level %u", addr, level);
	} else {
		LOG_WRN("Security failed: %s level %u err %d", addr, level, err);
	}
}
#endif

BT_CONN_CB_DEFINE(conn_callbacks) = {
	.connected    = connected,
	.disconnected = disconnected,
#ifdef CONFIG_BT_NUS_SECURITY_ENABLED
	.security_changed = security_changed,
#endif
};

#if defined(CONFIG_BT_NUS_SECURITY_ENABLED)
static void auth_passkey_display(struct bt_conn *conn, unsigned int passkey)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	LOG_INF("Passkey for %s: %06u", addr, passkey);
}

static void auth_passkey_confirm(struct bt_conn *conn, unsigned int passkey)
{
	char addr[BT_ADDR_LE_STR_LEN];

	auth_conn = bt_conn_ref(conn);

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	LOG_INF("Passkey for %s: %06u", addr, passkey);
	LOG_INF("Press Button 1 to confirm, Button 2 to reject.");
}


static void auth_cancel(struct bt_conn *conn)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	LOG_INF("Pairing cancelled: %s", addr);
}


static void pairing_complete(struct bt_conn *conn, bool bonded)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	LOG_INF("Pairing completed: %s, bonded: %d", addr, bonded);
}


static void pairing_failed(struct bt_conn *conn, enum bt_security_err reason)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	LOG_INF("Pairing failed conn: %s, reason %d", addr, reason);
}


static struct bt_conn_auth_cb conn_auth_callbacks = {
	.passkey_display = auth_passkey_display,
	.passkey_confirm = auth_passkey_confirm,
	.cancel = auth_cancel,
};

static struct bt_conn_auth_info_cb conn_auth_info_callbacks = {
	.pairing_complete = pairing_complete,
	.pairing_failed = pairing_failed,
};
#else
static struct bt_conn_auth_cb conn_auth_callbacks;
static struct bt_conn_auth_info_cb conn_auth_info_callbacks;
#endif

const uint8_t *_bt_receive_cb_data_buffer;
uint16_t _bt_receive_cb_data_buffer_length = 0;

static void bt_receive_cb(
    struct bt_conn *conn, 
    const uint8_t *const data,
    const uint16_t len
)
{
	char addr[BT_ADDR_LE_STR_LEN] = {0};

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, ARRAY_SIZE(addr));

	LOG_INF("Received data from: %s", addr);

	// k_mutex_lock(&_wait_new_packet_received_mutex, K_FOREVER);
	_bt_receive_cb_data_buffer = data;
	_bt_receive_cb_data_buffer_length = len;
    k_condvar_broadcast(&_wait_new_packet_received_condvar);
    // k_mutex_unlock(&_wait_new_packet_received_mutex);

	return;
}

static struct bt_nus_cb nus_cb = {
	.received = bt_receive_cb,
};

int peripheral_uart_init(void)
{
    int err = 0;

    if (IS_ENABLED(CONFIG_BT_NUS_SECURITY_ENABLED)) {
		err = bt_conn_auth_cb_register(&conn_auth_callbacks);
		if (err) {
			printk("Failed to register authorization callbacks.\n");
			return err;
		}

		err = bt_conn_auth_info_cb_register(&conn_auth_info_callbacks);
		if (err) {
			printk("Failed to register authorization info callbacks.\n");
			return err;
		}
	}

	err = bt_enable(NULL);
	if (err) {
        printk("Failed to bt_enable(NULL).\n");
        return err;
	}

	LOG_INF("Bluetooth initialized");

    // k_mutex_lock(&_init_mutex, K_FOREVER);
    _is_inited = true;
    k_condvar_broadcast(&_init_condvar);
    // k_mutex_unlock(&_init_mutex);

	if (IS_ENABLED(CONFIG_SETTINGS)) {
		settings_load();
	}

	err = bt_nus_init(&nus_cb);
	if (err) {
		LOG_ERR("Failed to initialize UART service (err: %d)", err);
		return 0;
	}

	err = bt_le_adv_start(BT_LE_ADV_CONN, ad, ARRAY_SIZE(ad), sd,
			      ARRAY_SIZE(sd));
	if (err) {
		LOG_ERR("Advertising failed to start (err %d)", err);
		return err;
	}

	return err;
}

#ifdef CONFIG_BT_NUS_SECURITY_ENABLED
void peripheral_uart_auth_conn(const bool accept)
{
	if (auth_conn) {
        if (accept) {
            bt_conn_auth_passkey_confirm(auth_conn);
            LOG_INF("Numeric Match, conn %p", (void *)auth_conn);
        } else {
            bt_conn_auth_cancel(auth_conn);
            LOG_INF("Numeric Reject, conn %p", (void *)auth_conn);
        }
        bt_conn_unref(auth_conn);
        auth_conn = NULL;
	}
}
#endif

BLE_SIMPLE_ERROR BLE_SIMPLE_IMPL_NRF_init(void)
{
	return peripheral_uart_init();
}
bool BLE_SIMPLE_IMPL_NRF_is_inited(void)
{
	return _is_inited;
}
void BLE_SIMPLE_IMPL_NRF_wait_inited(void)
{
    k_mutex_lock(&_init_mutex, K_FOREVER);
    while (!_is_inited) {
        k_condvar_wait(&_init_condvar, &_init_mutex, K_FOREVER);
    }
    k_mutex_unlock(&_init_mutex);
	return;
}

bool BLE_SIMPLE_is_connected(void)
{
	return _is_connected;
}

void BLE_SIMPLE_wait_connected(void)
{
	k_mutex_lock(&_connection_mutex, K_FOREVER);
    while (!_is_connected) {
        k_condvar_wait(&_connection_condvar, &_connection_mutex, K_FOREVER);
    }
    k_mutex_unlock(&_connection_mutex);
	return;
}

void BLE_SIMPLE_wait_disconnected(void)
{
	k_mutex_lock(&_connection_mutex, K_FOREVER);
    while (_is_connected) {
        k_condvar_wait(&_connection_condvar, &_connection_mutex, K_FOREVER);
    }
    k_mutex_unlock(&_connection_mutex);
	return;
}

void BLE_SIMPLE_wait_connection_state_changed(void)
{
    k_mutex_lock(&_connection_mutex, K_FOREVER);
	k_condvar_wait(&_connection_condvar, &_connection_mutex, K_FOREVER);
    k_mutex_unlock(&_connection_mutex);
	return;
}

BLE_SIMPLE_ERROR BLE_SIMPLE_wait_new_packet_received(void)
{
    k_mutex_lock(&_wait_new_packet_received_mutex, K_FOREVER);
	k_condvar_wait(&_wait_new_packet_received_condvar, &_wait_new_packet_received_mutex, K_FOREVER);
    k_mutex_unlock(&_wait_new_packet_received_mutex);
	return 0;
}

BLE_SIMPLE_ERROR BLE_SIMPLE_read_packet(
	const uint16_t packet_max_length,
    uint8_t *const packet,
    uint16_t *const packet_length
)
{
	k_mutex_lock(&_wait_new_packet_received_mutex, K_FOREVER);
	if(_bt_receive_cb_data_buffer_length > packet_max_length) {
    	k_mutex_unlock(&_wait_new_packet_received_mutex);
		return 1;
	}
	*packet_length = _bt_receive_cb_data_buffer_length;
	memcpy(packet, _bt_receive_cb_data_buffer, *packet_length);
    k_mutex_unlock(&_wait_new_packet_received_mutex);
	return 0;
}

BLE_SIMPLE_ERROR BLE_SIMPLE_send_packet(
    const uint8_t *const packet, 
    const uint16_t packet_length
)
{
	return bt_nus_send(NULL, packet, packet_length);
}

uint16_t BLE_SIMPLE_get_packet_max_length(void)
{
	return CONFIG_BT_L2CAP_TX_MTU - 4;
}
