///////////////////////////////////////////////////////////////////////////////////
// File: gatt.c
//
// Subject: Standardy i Systemy Komunikacyjne, AGH, EiT
//
// Author: Paweł Majewski
//
///////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////
// HEDER FILES
///////////////////////////////////////////////////////////////////////////////////

#include "gatt.h"

///////////////////////////////////////////////////////////////////////////////////
// DEKLARATION LOCAL FUNCTIONS
///////////////////////////////////////////////////////////////////////////////////

static void gatts_profile_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);

///////////////////////////////////////////////////////////////////////////////////
// LOCAL DATA
///////////////////////////////////////////////////////////////////////////////////

static struct gatts_profile_inst spp_profile_tab[SPP_PROFILE_NUM] = {
    [SPP_PROFILE_APP_IDX] = {
        .gatts_cb = gatts_profile_event_handler,
        .gatts_if = ESP_GATT_IF_NONE,
    },
};

///////////////////////////////////////////////////////////////////////////////////
// LOCAL FUNCTIONS
///////////////////////////////////////////////////////////////////////////////////

static void gatts_profile_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    ESP_LOGI(GATT_TAG, "event = %x",event);

    switch (event)
    {
    case ESP_GATTS_REG_EVT:

        break;
    case ESP_GATTS_READ_EVT:
        break;
    case ESP_GATTS_WRITE_EVT:
        break;
    case ESP_GATTS_EXEC_WRITE_EVT:
        break;
    case ESP_GATTS_MTU_EVT:
        break;
    case ESP_GATTS_CONF_EVT:
        break;
    case ESP_GATTS_UNREG_EVT:
        break;
    case ESP_GATTS_CREATE_EVT:
        break;
    case ESP_GATTS_ADD_INCL_SRVC_EVT:
        break;
    case ESP_GATTS_ADD_CHAR_EVT:
        break;
    case ESP_GATTS_ADD_CHAR_DESCR_EVT:
        break;
    case ESP_GATTS_DELETE_EVT:
        break;
    case ESP_GATTS_START_EVT:
        break;
    case ESP_GATTS_STOP_EVT:
        break;
    case ESP_GATTS_CONNECT_EVT:
        break;
    case ESP_GATTS_DISCONNECT_EVT:
        break;   
    case ESP_GATTS_OPEN_EVT:
        break;
    case ESP_GATTS_CANCEL_OPEN_EVT:
        break;
    case ESP_GATTS_CLOSE_EVT:
        break;
    case ESP_GATTS_LISTEN_EVT:
        break;
    case ESP_GATTS_CONGEST_EVT:
        break;
    case ESP_GATTS_RESPONSE_EVT:
        break;
    case ESP_GATTS_CREAT_ATTR_TAB_EVT:
        break;
    case ESP_GATTS_SET_ATTR_VAL_EVT:
        break;
    case ESP_GATTS_SEND_SERVICE_CHANGE_EVT:
        break;       
    default:
        break;
    }
}
///////////////////////////////////////////////////////////////////////////////////
// GLOBAL FUNCTIONS
///////////////////////////////////////////////////////////////////////////////////

void gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    esp_err_t error;
    ESP_LOGE(GATT_TAG, "GATT_EVT, event %d, gatts if %d", event, gatts_if);

    switch (event)
    {
    case ESP_GATTS_REG_EVT:
        if (param->reg.status == ESP_GATT_OK)
        {
            spp_profile_tab[SPP_PROFILE_APP_IDX].gatts_if = gatts_if;
        }
        else
        {
            ESP_LOGI(GATT_TAG, "Reg app failed, app_id %04x, status %d",param->reg.app_id, param->reg.status);
            return;
        }
        break;
    case ESP_GATTS_READ_EVT:
        break;
    case ESP_GATTS_WRITE_EVT:
        break;
    case ESP_GATTS_EXEC_WRITE_EVT:
        break;
    case ESP_GATTS_MTU_EVT:
        break;
    case ESP_GATTS_CONF_EVT:
        break;
    case ESP_GATTS_UNREG_EVT:
        break;
    case ESP_GATTS_CREATE_EVT:
        break;
    case ESP_GATTS_ADD_INCL_SRVC_EVT:
        break;
    case ESP_GATTS_ADD_CHAR_EVT:
        break;
    case ESP_GATTS_ADD_CHAR_DESCR_EVT:
        break;
    case ESP_GATTS_DELETE_EVT:
        break;
    case ESP_GATTS_START_EVT:
        break;
    case ESP_GATTS_STOP_EVT:
        break;
    case ESP_GATTS_CONNECT_EVT:
        break;
    case ESP_GATTS_DISCONNECT_EVT:
        break;   
    case ESP_GATTS_OPEN_EVT:
        break;
    case ESP_GATTS_CANCEL_OPEN_EVT:
        break;
    case ESP_GATTS_CLOSE_EVT:
        break;
    case ESP_GATTS_LISTEN_EVT:
        break;
    case ESP_GATTS_CONGEST_EVT:
        break;
    case ESP_GATTS_RESPONSE_EVT:
        break;
    case ESP_GATTS_CREAT_ATTR_TAB_EVT:
        break;
    case ESP_GATTS_SET_ATTR_VAL_EVT:
        break;
    case ESP_GATTS_SEND_SERVICE_CHANGE_EVT:
        break;       
    default:
        break;
    }

    int idx;
    for (idx = 0; idx < SPP_PROFILE_NUM; idx++)
    {
        if (gatts_if == ESP_GATT_IF_NONE  || gatts_if == spp_profile_tab[idx].gatts_if)
        {
            if (spp_profile_tab[idx].gatts_cb)
            {
                spp_profile_tab[idx].gatts_cb(event, gatts_if, param);
            }
        }
    }
}
