///////////////////////////////////////////////////////////////////////////////////
// File: gap.c
//
// Subject: Standardy i Systemy Komunikacyjne, AGH, EiT
//
// Author: Paweł Majewski
//
///////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////
// HEDER FILES
///////////////////////////////////////////////////////////////////////////////////

#include "gap.h"

///////////////////////////////////////////////////////////////////////////////////
// LOCAL DATA
///////////////////////////////////////////////////////////////////////////////////

//Advertising parameters
static esp_ble_adv_params_t spp_adv_params = {
    .adv_int_min        = 0x20,
    .adv_int_max        = 0x40,
    .adv_type           = ADV_TYPE_IND,
    .own_addr_type      = BLE_ADDR_TYPE_PUBLIC,
    .channel_map        = ADV_CHNL_ALL,
    .adv_filter_policy  = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

///////////////////////////////////////////////////////////////////////////////////
// GLOBAL FUNCTIONS
///////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Bluetooth Generic Access Profile (GAP) event handler.
 *
 * This function handles events related to the Bluetooth Low Energy (BLE) Generic Access Profile.
 * Specifically, it responds to the completion event of setting raw advertising data by restarting advertising.
 *
 * @param[in] event   The specific GAP BLE callback event.
 * @param[in] param   Pointer to the callback parameters containing event-specific data.
 */
void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    esp_err_t error;
    ESP_LOGE(GAP_TAG, "GAP_EVT, event %d", event);

    switch (event)
    {
#if (BLE_42_FEATURE_SUPPORT == TRUE)
    case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
        break;
    case ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT:
        break;
    case ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT:
        break;
    case ESP_GAP_BLE_SCAN_RESULT_EVT:
        break; 
    case ESP_GAP_BLE_ADV_DATA_RAW_SET_COMPLETE_EVT:
        if (esp_ble_gap_start_advertising(&spp_adv_params) == ESP_OK)
        {
            ESP_LOGE(GAP_TAG, "Set advertising params successful.");
        }
        else
        {
            ESP_LOGE(GAP_TAG, "Set advertising params unsuccessful.");
            return;
        }
        break;
    case ESP_GAP_BLE_SCAN_RSP_DATA_RAW_SET_COMPLETE_EVT:
        break;
    case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
        if ((error = param->adv_start_cmpl.status) != ESP_BT_STATUS_SUCCESS)
        {
            ESP_LOGE(GAP_TAG, "Advertising start failed: %s", esp_err_to_name(error));
            return;
        }
        break;
    case ESP_GAP_BLE_SCAN_START_COMPLETE_EVT:
        break; 
#endif /* BLE_42_FEATURE_SUPPORT == TRUE */

    case ESP_GAP_BLE_AUTH_CMPL_EVT:
        break;
    case ESP_GAP_BLE_KEY_EVT:
        break;
    case ESP_GAP_BLE_SEC_REQ_EVT:
        break;
    case ESP_GAP_BLE_PASSKEY_NOTIF_EVT:
        break;  
    case ESP_GAP_BLE_PASSKEY_REQ_EVT:
        break;
    case ESP_GAP_BLE_OOB_REQ_EVT:
        break;
    case ESP_GAP_BLE_LOCAL_IR_EVT:
        break;
    case ESP_GAP_BLE_LOCAL_ER_EVT:
        break; 
    case ESP_GAP_BLE_NC_REQ_EVT:
        break;

#if (BLE_42_FEATURE_SUPPORT == TRUE)
    case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT:
        break;
    case ESP_GAP_BLE_SCAN_STOP_COMPLETE_EVT:
        break;
#endif /* BLE_42_FEATURE_SUPPORT == TRUE */

    case ESP_GAP_BLE_SET_STATIC_RAND_ADDR_EVT:
        break; 
    case ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT:
        break;
    case ESP_GAP_BLE_SET_PKT_LENGTH_COMPLETE_EVT:
        break;
    case ESP_GAP_BLE_SET_LOCAL_PRIVACY_COMPLETE_EVT:
        break;
    case ESP_GAP_BLE_REMOVE_BOND_DEV_COMPLETE_EVT:
        break; 
    case ESP_GAP_BLE_CLEAR_BOND_DEV_COMPLETE_EVT:
        break;
    case ESP_GAP_BLE_GET_BOND_DEV_COMPLETE_EVT:
        break;
    case ESP_GAP_BLE_READ_RSSI_COMPLETE_EVT:
        break;
    case ESP_GAP_BLE_UPDATE_WHITELIST_COMPLETE_EVT:
        break; 

#if (BLE_42_FEATURE_SUPPORT == TRUE)
    case ESP_GAP_BLE_UPDATE_DUPLICATE_EXCEPTIONAL_LIST_COMPLETE_EVT:
        break;
#endif /* BLE_42_FEATURE_SUPPORT == TRUE */

    case ESP_GAP_BLE_SET_CHANNELS_EVT:
        break;

#if (BLE_50_FEATURE_SUPPORT == TRUE)
    case ESP_GAP_BLE_READ_PHY_COMPLETE_EVT:
        break;
    case ESP_GAP_BLE_SET_PREFERRED_DEFAULT_PHY_COMPLETE_EVT:
        break; 
    case ESP_GAP_BLE_SET_PREFERRED_PHY_COMPLETE_EVT:
        break;
    case ESP_GAP_BLE_EXT_ADV_SET_RAND_ADDR_COMPLETE_EVT:
        break;
    case ESP_GAP_BLE_EXT_ADV_SET_PARAMS_COMPLETE_EVT:
        break;
    case ESP_GAP_BLE_EXT_ADV_DATA_SET_COMPLETE_EVT:
        break;  
    case ESP_GAP_BLE_EXT_SCAN_RSP_DATA_SET_COMPLETE_EVT:
        break;
    case ESP_GAP_BLE_EXT_ADV_START_COMPLETE_EVT:
        break; 
    case ESP_GAP_BLE_EXT_ADV_STOP_COMPLETE_EVT:
        break;
    case ESP_GAP_BLE_EXT_ADV_SET_REMOVE_COMPLETE_EVT:
        break;
    case ESP_GAP_BLE_EXT_ADV_SET_CLEAR_COMPLETE_EVT:
        break;
    case ESP_GAP_BLE_PERIODIC_ADV_SET_PARAMS_COMPLETE_EVT:
        break; 
    case ESP_GAP_BLE_PERIODIC_ADV_DATA_SET_COMPLETE_EVT:
        break;
    case ESP_GAP_BLE_PERIODIC_ADV_START_COMPLETE_EVT:
        break; 
    case ESP_GAP_BLE_PERIODIC_ADV_STOP_COMPLETE_EVT:
        break;
    case ESP_GAP_BLE_PERIODIC_ADV_CREATE_SYNC_COMPLETE_EVT:
        break;
    case ESP_GAP_BLE_PERIODIC_ADV_SYNC_CANCEL_COMPLETE_EVT:
        break;
    case ESP_GAP_BLE_PERIODIC_ADV_SYNC_TERMINATE_COMPLETE_EVT:
        break; 
    case ESP_GAP_BLE_PERIODIC_ADV_ADD_DEV_COMPLETE_EVT:
        break;
    case ESP_GAP_BLE_PERIODIC_ADV_REMOVE_DEV_COMPLETE_EVT:
        break; 
    case ESP_GAP_BLE_PERIODIC_ADV_CLEAR_DEV_COMPLETE_EVT:
        break;
    case ESP_GAP_BLE_SET_EXT_SCAN_PARAMS_COMPLETE_EVT:
        break;
    case ESP_GAP_BLE_EXT_SCAN_START_COMPLETE_EVT:
        break;
    case ESP_GAP_BLE_EXT_SCAN_STOP_COMPLETE_EVT:
        break; 
    case ESP_GAP_BLE_PREFER_EXT_CONN_PARAMS_SET_COMPLETE_EVT:
        break;
    case ESP_GAP_BLE_PHY_UPDATE_COMPLETE_EVT:
        break; 
    case ESP_GAP_BLE_EXT_ADV_REPORT_EVT:
        break;
    case ESP_GAP_BLE_SCAN_TIMEOUT_EVT:
        break;
    case ESP_GAP_BLE_ADV_TERMINATED_EVT:
        break;
    case ESP_GAP_BLE_SCAN_REQ_RECEIVED_EVT:
        break; 
    case ESP_GAP_BLE_CHANNEL_SELECT_ALGORITHM_EVT:
        break;
    case ESP_GAP_BLE_PERIODIC_ADV_REPORT_EVT:
        break; 
    case ESP_GAP_BLE_PERIODIC_ADV_SYNC_LOST_EVT:
        break;
    case ESP_GAP_BLE_PERIODIC_ADV_SYNC_ESTAB_EVT:
        break;
#endif /* BLE_50_FEATURE_SUPPORT == TRUE */

    case ESP_GAP_BLE_EVT_MAX:
        break;     
    default:
        break;
    }
}
