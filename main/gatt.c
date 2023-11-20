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

static const esp_gatts_attr_db_t spp_gatt_db[SPP_IDX_NB] =
{
//TODO
};

static const uint8_t spp_adv_data[19] = {
    /* Flags */
    0x02,0x01,0x06,
    /* Complete List of 16-bit Service Class UUIDs */
    0x03,0x03,0xF0,0xAB,
    /* Complete Local Name in advertising */
    0x0F,0x09, 'S', 'P', 'P', '_', 'S', 'E', 'R','V', 'E', 'R'
};

static uint16_t spp_handle_table[SPP_IDX_NB];
static bool enable_data_ntf =           false;
static uint16_t spp_mtu_size =          23;
static uint16_t spp_conn_id =           0xffff;
static esp_gatt_if_t spp_gatts_if =     0xff;
static bool is_con =                    false;
static esp_bd_addr_t spp_remote_bda =   {0x0,};
static QueueHandle_t cmd_cmd_queue =    NULL;

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
// LOCAL FUNCTIONS
///////////////////////////////////////////////////////////////////////////////////

static uint8_t find_char_and_desr_index(uint16_t handle)
{
    uint8_t error = 0xff;

    for(int i = 0; i < SPP_IDX_NB ; i++){
        if(handle == spp_handle_table[i]){
            return i;
        }
    }

    return error;
}

static bool store_wr_buffer(esp_ble_gatts_cb_param_t *p_data)
{
    //TODO
    return true;
}

static void print_write_buffer(void)
{

}

static void free_write_buffer(void)
{

}

static void gatts_profile_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    esp_err_t error;
    esp_ble_gatts_cb_param_t *p_param = (esp_ble_gatts_cb_param_t *) param;

    ESP_LOGI(GATT_TAG, "event = %x",event);

    switch (event)
    {
    case ESP_GATTS_REG_EVT:
        error = esp_ble_gatts_create_attr_tab(spp_gatt_db, gatts_if, SPP_IDX_NB, SPP_INST_ID);
        if (error) {
            ESP_LOGE(GATT_TAG, "Create attr table failed, error code = 0x%x.", error);
            return;
        }
        error = esp_ble_gap_set_device_name(DEVICE_NAME);
        if (error) {
            ESP_LOGE(GATT_TAG, "Set device name failed, error code = 0x%x.", error);
            return;
        } 
        error = esp_ble_gap_config_adv_data_raw((uint8_t*)spp_adv_data, sizeof(spp_adv_data));
        if (error) {
            ESP_LOGE(GATT_TAG, "Config raw adv data failed, error code = 0x%x.", error);
            return;  
        }      
        break;
    case ESP_GATTS_READ_EVT:
        error = find_char_and_desr_index(p_param->read.handle);
        if (error == 0xff) {
            ESP_LOGE(GATT_TAG, "Error in ESP_GATTS_READ_EVT.");
            return;  
        }         
        break;
    case ESP_GATTS_WRITE_EVT:
        error = find_char_and_desr_index(p_param->write.handle);
        if (error == 0xff) {
            ESP_LOGE(GATT_TAG, "Error in ESP_GATTS_WRITE_EVT.");
            return;  
        }       
        if (p_param->write.is_prep == false)
        {
            if (error == SPP_IDX_SPP_COMMAND_VAL)
            {
                uint8_t * spp_cmd_buff = NULL;
                spp_cmd_buff = (uint8_t *)malloc((spp_mtu_size - 3) * sizeof(uint8_t));

                if (spp_cmd_buff == NULL)
                {
                    ESP_LOGE(GATT_TAG, "Malloc failed.");
                    break;
                }
                memset(spp_cmd_buff, 0x0, (spp_mtu_size - 3));
                memcpy(spp_cmd_buff, p_param->write.value, p_param->write.len);
                xQueueSend(cmd_cmd_queue, &spp_cmd_buff, (10 / portTICK_PERIOD_MS));
            }
            else if (error == SPP_IDX_SPP_DATA_NTF_CFG)
            {
                if ((p_param->write.len == 2) && (p_param->write.value[0] == 0x01) && (p_param->write.value[1] == 0x00))
                {
                    enable_data_ntf = true;
                }
                else if ((p_param->write.len == 2) && (p_param->write.value[0] == 0x00) && (p_param->write.value[1] == 0x00))
                {
                    enable_data_ntf = false;
                }
            }
            else if (error == SPP_IDX_SPP_DATA_RECV_VAL)
            {
                uart_write_bytes(UART_NUM_0, (char *)(p_param->write.value), p_param->write.len);
            }
        }
        else if ((p_param->write.is_prep == true) && (error == SPP_IDX_SPP_DATA_RECV_VAL))
        {
            ESP_LOGI(GATT_TAG, "ESP_GATTS_PREP_WRITE_EVT : handle = %d", error);
            store_wr_buffer(p_param);
        }

        break;
    case ESP_GATTS_EXEC_WRITE_EVT:
    	    if(p_param->exec_write.exec_write_flag){
    	        print_write_buffer();
    	        free_write_buffer();
    	    }        
        break;
    case ESP_GATTS_MTU_EVT:
        spp_mtu_size = p_param->mtu.mtu;
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
        spp_conn_id = p_param->connect.conn_id;
        spp_gatts_if = gatts_if;
        is_con = true;
        memcpy(&spp_remote_bda, &p_param->connect.remote_bda, sizeof(esp_bd_addr_t));
        break;
    case ESP_GATTS_DISCONNECT_EVT:
        is_con = false;
        enable_data_ntf = false;
        esp_ble_gap_start_advertising(&spp_adv_params);
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
        if ((param->add_attr_tab.status != ESP_GATT_OK) && (param->add_attr_tab.num_handle != SPP_IDX_NB))
        {
             ESP_LOGE(GATT_TAG, "Error ESP_GATTS_CREAT_ATTR_TAB_EVT");
             return;
        }
        else
        {
            memcpy(spp_handle_table, param->add_attr_tab.handles, sizeof(spp_handle_table));
            esp_ble_gatts_start_service(spp_handle_table[SPP_IDX_SVC]);
        }
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
