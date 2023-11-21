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

// Service Declaration
static const uint16_t primary_service_uuid = ESP_GATT_UUID_PRI_SERVICE;
static const uint16_t spp_service_uuid = 0xABF0;

// SPP -  data receive characteristic Declaration
static const uint16_t character_declaration_uuid = ESP_GATT_UUID_CHAR_DECLARE;
static const uint8_t char_prop_read_write = ESP_GATT_CHAR_PROP_BIT_WRITE_NR|ESP_GATT_CHAR_PROP_BIT_READ;

//SPP -  data receive characteristic Value
static const uint16_t spp_data_receive_uuid = ESP_GATT_UUID_SPP_DATA_RECEIVE;
static const uint8_t  spp_data_receive_val[20] = {0x00};

// SPP -  data notify characteristic Declaration
static const uint8_t char_prop_read_notify = ESP_GATT_CHAR_PROP_BIT_READ|ESP_GATT_CHAR_PROP_BIT_NOTIFY;

// SPP -  data notify characteristic Value
static const uint16_t spp_data_notify_uuid = ESP_GATT_UUID_SPP_DATA_NOTIFY;
static const uint8_t  spp_data_notify_val[20] = {0x00};

// SPP -  data notify characteristic - Client Characteristic Configuration Descriptor
static const uint16_t character_client_config_uuid = ESP_GATT_UUID_CHAR_CLIENT_CONFIG;
static const uint8_t  spp_data_notify_ccc[2] = {0x00, 0x00};

// SPP -  command characteristic Value
static const uint16_t spp_command_uuid = ESP_GATT_UUID_SPP_COMMAND_RECEIVE;
static const uint8_t  spp_command_val[10] = {0x00};

// SPP -  status characteristic Value
static const uint16_t spp_status_uuid = ESP_GATT_UUID_SPP_COMMAND_NOTIFY;
static const uint8_t  spp_status_val[10] = {0x00};

// SPP -  status characteristic - Client Characteristic Configuration Descriptor
static const uint8_t  spp_status_ccc[2] = {0x00, 0x00};

static const esp_gatts_attr_db_t spp_gatt_db[SPP_IDX_NB] =
{
    // SPP -  Service Declaration
    [SPP_IDX_SVC]                      	=
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&primary_service_uuid, ESP_GATT_PERM_READ,
    sizeof(spp_service_uuid), sizeof(spp_service_uuid), (uint8_t *)&spp_service_uuid}},

    // SPP -  data receive characteristic Declaration
    [SPP_IDX_SPP_DATA_RECV_CHAR]            =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ,
    CHAR_DECLARATION_SIZE,CHAR_DECLARATION_SIZE, (uint8_t *)&char_prop_read_write}},

    // SPP -  data receive characteristic Value
    [SPP_IDX_SPP_DATA_RECV_VAL]             	=
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&spp_data_receive_uuid, ESP_GATT_PERM_READ|ESP_GATT_PERM_WRITE,
    SPP_DATA_MAX_LEN,sizeof(spp_data_receive_val), (uint8_t *)spp_data_receive_val}},

    // SPP -  data notify characteristic Declaration
    [SPP_IDX_SPP_DATA_NOTIFY_CHAR]  =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ,
    CHAR_DECLARATION_SIZE,CHAR_DECLARATION_SIZE, (uint8_t *)&char_prop_read_notify}},

    // SPP -  data notify characteristic Value
    [SPP_IDX_SPP_DATA_NTY_VAL]   =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&spp_data_notify_uuid, ESP_GATT_PERM_READ,
    SPP_DATA_MAX_LEN, sizeof(spp_data_notify_val), (uint8_t *)spp_data_notify_val}},

    // SPP -  data notify characteristic - Client Characteristic Configuration Descriptor
    [SPP_IDX_SPP_DATA_NTF_CFG]         =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_client_config_uuid, ESP_GATT_PERM_READ|ESP_GATT_PERM_WRITE,
    sizeof(uint16_t),sizeof(spp_data_notify_ccc), (uint8_t *)spp_data_notify_ccc}},

    // SPP -  command characteristic Declaration
    [SPP_IDX_SPP_COMMAND_CHAR]            =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ,
    CHAR_DECLARATION_SIZE,CHAR_DECLARATION_SIZE, (uint8_t *)&char_prop_read_write}},

    // SPP -  command characteristic Value
    [SPP_IDX_SPP_COMMAND_VAL]                 =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&spp_command_uuid, ESP_GATT_PERM_READ|ESP_GATT_PERM_WRITE,
    SPP_CMD_MAX_LEN,sizeof(spp_command_val), (uint8_t *)spp_command_val}},

    // SPP -  status characteristic Declaration
    [SPP_IDX_SPP_STATUS_CHAR]            =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ,
    CHAR_DECLARATION_SIZE,CHAR_DECLARATION_SIZE, (uint8_t *)&char_prop_read_notify}},

    // SPP -  status characteristic Value
    [SPP_IDX_SPP_STATUS_VAL]                 =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&spp_status_uuid, ESP_GATT_PERM_READ,
    SPP_STATUS_MAX_LEN,sizeof(spp_status_val), (uint8_t *)spp_status_val}},

    // SPP -  status characteristic - Client Characteristic Configuration Descriptor
    [SPP_IDX_SPP_STATUS_CFG]         =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_client_config_uuid, ESP_GATT_PERM_READ|ESP_GATT_PERM_WRITE,
    sizeof(uint16_t),sizeof(spp_status_ccc), (uint8_t *)spp_status_ccc}},
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

static spp_receive_data_node_t * temp_spp_recv_data_node_p1 = NULL;
static spp_receive_data_node_t * temp_spp_recv_data_node_p2 = NULL;

static spp_receive_data_buff_t SppRecvDataBuff = {
    .node_num   = 0,
    .buff_size  = 0,
    .first_node = NULL
};

QueueHandle_t spp_uart_queue = NULL;

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

static bool store_wr_buffer(esp_ble_gatts_cb_param_t *p_param)
{
    temp_spp_recv_data_node_p1 = (spp_receive_data_node_t *)malloc(sizeof(spp_receive_data_node_t));
    if (temp_spp_recv_data_node_p1 == NULL)
    {
        ESP_LOGI(GATT_TAG, "Malloc failed.");
        return false;
    }

    if (temp_spp_recv_data_node_p2 != NULL)
    {
        temp_spp_recv_data_node_p2->next_node = temp_spp_recv_data_node_p1;
    }
    temp_spp_recv_data_node_p1->len = p_param->write.len;
    SppRecvDataBuff.buff_size += p_param->write.len;
    temp_spp_recv_data_node_p1->next_node = NULL;
    temp_spp_recv_data_node_p1->node_buff = (uint8_t *)malloc(p_param->write.len);
    temp_spp_recv_data_node_p2 = temp_spp_recv_data_node_p1;
    if (temp_spp_recv_data_node_p1->node_buff == NULL)
    {
        ESP_LOGI(GATT_TAG, "Malloc failed.");
        temp_spp_recv_data_node_p1->len = 0;
    }
    else
    {
        memcpy(temp_spp_recv_data_node_p1->node_buff, p_param->write.value, p_param->write.len);
    }
    
    if (SppRecvDataBuff.node_num == 0)
    {
        SppRecvDataBuff.first_node = temp_spp_recv_data_node_p1;
        SppRecvDataBuff.node_num++;
    }
    else
    {
        SppRecvDataBuff.node_num++;
    }

    return true;
}

static void print_write_buffer(void)
{
    temp_spp_recv_data_node_p1 = SppRecvDataBuff.first_node;

    while(temp_spp_recv_data_node_p1 != NULL){
        uart_write_bytes(UART_NUM_0, (char *)(temp_spp_recv_data_node_p1->node_buff), temp_spp_recv_data_node_p1->len);
        temp_spp_recv_data_node_p1 = temp_spp_recv_data_node_p1->next_node;
    }
}

static void free_write_buffer(void)
{
    temp_spp_recv_data_node_p1 = SppRecvDataBuff.first_node;

    while(temp_spp_recv_data_node_p1 != NULL){
        temp_spp_recv_data_node_p2 = temp_spp_recv_data_node_p1->next_node;
        if (temp_spp_recv_data_node_p1->node_buff) {
            free(temp_spp_recv_data_node_p1->node_buff);
        }
        free(temp_spp_recv_data_node_p1);
        temp_spp_recv_data_node_p1 = temp_spp_recv_data_node_p2;
    }

    SppRecvDataBuff.node_num = 0;
    SppRecvDataBuff.buff_size = 0;
    SppRecvDataBuff.first_node = NULL;
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

static void uart_task(void *pvParameters)
{
    uart_event_t event;
    uint8_t total_num = 0;
    uint8_t current_num = 0;

    for (;;) {
        //Waiting for UART event.
        if (xQueueReceive(spp_uart_queue, (void * )&event, (TickType_t)portMAX_DELAY)) 
        {
            switch (event.type) 
            {
            //Event of UART receving data
            case UART_DATA:
                if ((event.size)&&(is_con)) 
                {
                    uint8_t * temp = NULL;
                    uint8_t * ntf_value_p = NULL;

                    if(!enable_data_ntf)
                    {
                        ESP_LOGE(SPP_TAG, "Notify do not enable.");
                        break;
                    }
                    temp = (uint8_t *)malloc(sizeof(uint8_t) * event.size);
                    if(temp == NULL)
                    {
                        ESP_LOGE(SPP_TAG, "Malloc failed.");
                        break;
                    }
                    memset(temp,0x0,event.size);
                    uart_read_bytes(UART_NUM_0,temp,event.size,portMAX_DELAY);
                    if(event.size <= (spp_mtu_size - 3))
                    {
                        esp_ble_gatts_send_indicate(spp_gatts_if, spp_conn_id, spp_handle_table[SPP_IDX_SPP_DATA_NTY_VAL],event.size, temp, false);
                    }else if(event.size > (spp_mtu_size - 3))
                    {
                        if((event.size%(spp_mtu_size - 7)) == 0)
                        {
                            total_num = event.size / (spp_mtu_size - 7);
                        }else
                        {
                            total_num = event.size / (spp_mtu_size - 7) + 1;
                        }
                        current_num = 1;
                        ntf_value_p = (uint8_t *)malloc((spp_mtu_size - 3) * sizeof(uint8_t));
                        if(ntf_value_p == NULL)
                        {
                            ESP_LOGE(SPP_TAG, "Malloc failed.");
                            free(temp);
                            break;
                        }
                        while(current_num <= total_num)
                        {
                            if(current_num < total_num)
                            {
                                ntf_value_p[0] = '#';
                                ntf_value_p[1] = '#';
                                ntf_value_p[2] = total_num;
                                ntf_value_p[3] = current_num;
                                memcpy(ntf_value_p + 4,temp + (current_num - 1) * (spp_mtu_size - 7), (spp_mtu_size - 7));
                                esp_ble_gatts_send_indicate(spp_gatts_if, spp_conn_id, spp_handle_table[SPP_IDX_SPP_DATA_NTY_VAL], (spp_mtu_size-3), ntf_value_p, false);
                            }else if(current_num == total_num)
                            {
                                ntf_value_p[0] = '#';
                                ntf_value_p[1] = '#';
                                ntf_value_p[2] = total_num;
                                ntf_value_p[3] = current_num;
                                memcpy(ntf_value_p + 4,temp + (current_num - 1) * (spp_mtu_size - 7), (event.size - (current_num - 1) * (spp_mtu_size - 7)));
                                esp_ble_gatts_send_indicate(spp_gatts_if, spp_conn_id, spp_handle_table[SPP_IDX_SPP_DATA_NTY_VAL], (event.size - (current_num - 1) * (spp_mtu_size - 7) + 4), ntf_value_p, false);
                            }
                            vTaskDelay(20 / portTICK_PERIOD_MS);
                            current_num++;
                        }
                        free(ntf_value_p);
                    }
                    free(temp);
                }
                break;
            default:
                break;
            }
        }
    }
    vTaskDelete(NULL);
}

static void spp_uart_init(void)
{
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_RTS,
        .rx_flow_ctrl_thresh = 122,
        .source_clk = UART_SCLK_DEFAULT,
    };

    //Install UART driver, and get the queue.
    uart_driver_install(UART_NUM_0, 4096, 8192, 10, &spp_uart_queue, 0);
    //Set UART parameters
    uart_param_config(UART_NUM_0, &uart_config);
    //Set UART pins
    uart_set_pin(UART_NUM_0, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    xTaskCreate(uart_task, "uTask", 2048, (void*)UART_NUM_0, 8, NULL);
}

static void spp_cmd_task(void * arg)
{
    uint8_t * cmd_id;

    for(;;){
        vTaskDelay(50 / portTICK_PERIOD_MS);
        if(xQueueReceive(cmd_cmd_queue, &cmd_id, portMAX_DELAY)) {
            esp_log_buffer_char(SPP_TAG, (char*)(cmd_id), strlen((char*)cmd_id));
            free(cmd_id);
        }
    }
    vTaskDelete(NULL);
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

void spp_task_init(void)
{
    spp_uart_init();
    cmd_cmd_queue = xQueueCreate(10, sizeof(uint32_t));
    xTaskCreate(spp_cmd_task, "spp_cmd_task", 2048, NULL, 10, NULL);
}