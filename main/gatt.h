///////////////////////////////////////////////////////////////////////////////////
// File: gatt.h
//
// Subject: Standardy i Systemy Komunikacyjne, AGH, EiT
//
// Author: Paweł Majewski
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef GATT_H
#define GATT_H

///////////////////////////////////////////////////////////////////////////////////
// HEDER FILES
///////////////////////////////////////////////////////////////////////////////////

#include "ble_spp_server.h"

///////////////////////////////////////////////////////////////////////////////////
// PRAMETERS
///////////////////////////////////////////////////////////////////////////////////

#define GATT_TAG  "GATT"
#define SPP_TAG   "SPP"

#define SPP_PROFILE_NUM     1
#define SPP_PROFILE_APP_IDX 0

#define DEVICE_NAME "PM_SERVER"
#define SPP_INST_ID 0

#define ESP_SPP_APP_ID              0x56

#define CHAR_DECLARATION_SIZE   (sizeof(uint8_t))

/// Characteristic UUID
#define ESP_GATT_UUID_SPP_DATA_RECEIVE      0xABF1
#define ESP_GATT_UUID_SPP_DATA_NOTIFY       0xABF2
#define ESP_GATT_UUID_SPP_COMMAND_RECEIVE   0xABF3
#define ESP_GATT_UUID_SPP_COMMAND_NOTIFY    0xABF4

///////////////////////////////////////////////////////////////////////////////////
// DATA
///////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Structure representing a GATT Server profile instance.
 */
struct gatts_profile_inst {
    esp_gatts_cb_t gatts_cb;       /**< GATT Server callback function. */
    uint16_t gatts_if;             /**< GATT Server interface. */
    uint16_t app_id;               /**< Application ID. */
    uint16_t conn_id;              /**< Connection ID. */
    uint16_t service_handle;       /**< Service handle. */
    esp_gatt_srvc_id_t service_id; /**< Service ID. */
    uint16_t char_handle;          /**< Characteristic handle. */
    esp_bt_uuid_t char_uuid;       /**< Characteristic UUID. */
    esp_gatt_perm_t perm;          /**< Permissions for the characteristic. */
    esp_gatt_char_prop_t property; /**< Properties of the characteristic. */
    uint16_t descr_handle;         /**< Descriptor handle. */
    esp_bt_uuid_t descr_uuid;      /**< Descriptor UUID. */
};

/**
 * @brief Structure representing a node in the SPP receive data buffer.
 */
typedef struct spp_receive_data_node {
    int32_t len;                                /**< Length of the data in the node. */
    uint8_t *node_buff;                         /**< Pointer to the data buffer. */
    struct spp_receive_data_node *next_node;    /**< Pointer to the next node in the buffer. */
} spp_receive_data_node_t;

/**
 * @brief Structure to manage the buffer for receiving SPP data.
 */
typedef struct spp_receive_data_buff {
    int32_t node_num;                       /**< Number of nodes in the buffer. */
    int32_t buff_size;                      /**< Total size of the buffer. */
    spp_receive_data_node_t *first_node;    /**< Pointer to the first node in the buffer. */
} spp_receive_data_buff_t;

///////////////////////////////////////////////////////////////////////////////////
// GLOBAL FUNCTIONS
///////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Event handler for GATT server events.
 *
 * This function processes GATT server events and delegates them to the corresponding profile instance's callback.
 *
 * @param[in] event GATT server event.
 * @param[in] gatts_if GATT server interface.
 * @param[in] param GATT server event parameters.
 */
void gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);

/**
 * @brief Initialize the SPP task.
 *
 * This function initializes the SPP task, including UART initialization and task creation for command handling.
 */
void spp_task_init(void);

#endif /* GATT_H */