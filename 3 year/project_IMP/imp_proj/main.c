/*
***************************************************************
*               IMP project: Spínání světla dle intenzity     *     
*               Author: Kambulat Alakaev(xalaka00)            *
***************************************************************
*/

#include <stdio.h>
#include "driver/ledc.h"
#include "driver/i2c.h"
#include "driver/uart.h"
#include "nvs_flash.h"
#include "mqtt_client.h"
#include "esp_wifi.h"
#include "esp_event.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_system.h"
#include "esp_err.h"
#include "esp_log.h"
#include "unistd.h"



#define LEDC_OUTPUT_IO      14      // define the output GPIO
#define LEDC_FREQUENCY      5000    // set frequency at 5 kHz
#define BH1750_I2C_ADDRESS  0x23    // represents the I2C address of the BH1750
#define I2C_MASTER_FREQ_HZ  100000  // clock frequency for master mode
#define BUF_SIZE            1024    // size of a buffer
#define TAG "MQTT_APP"              // tag of the app for logging

// variable for saving the illuminance in lux from BH1750
uint16_t lux;


/**
 * @brief configure LED 
 * 
 */
void ledc_init()
{
    // prepare and  apply the LEDC PWM timer configuration
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_LOW_SPEED_MODE,
        .timer_num        = LEDC_TIMER_0,
        .duty_resolution  = LEDC_TIMER_13_BIT, // resolution of the duty
        .freq_hz          = LEDC_FREQUENCY,  // set output frequency at 5 kHz
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // prepare and apply the LEDC PWM channel configuration to generate waveform
    ledc_channel_config_t ledc_channel = {
        .speed_mode     = LEDC_LOW_SPEED_MODE,
        .channel        = LEDC_CHANNEL_0,
        .timer_sel      = LEDC_TIMER_0,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = LEDC_OUTPUT_IO,
        .duty           = 0, 
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
}


/**
 * @brief initialize and set bus of the I2C to communicate with BH1750
 * 
 */
void init_i2c_bus()
{
    i2c_config_t i2c_config = {
		.mode = I2C_MODE_MASTER,
		.sda_io_num = GPIO_NUM_21,
		.scl_io_num = GPIO_NUM_22,
		.sda_pullup_en = GPIO_PULLUP_ENABLE,
		.scl_pullup_en = GPIO_PULLUP_ENABLE,
		.master.clk_speed = I2C_MASTER_FREQ_HZ
	};
	ESP_ERROR_CHECK(i2c_param_config(I2C_NUM_0, &i2c_config));
	ESP_ERROR_CHECK(i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0));
}

/**
 * @brief enable and start communication with BH1750 
 * 
 */
void enable_bh1750(){
    // create an I2C command handler for reading data from the BH1750
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (BH1750_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);
    // Send a command to request illuminance data (e.g., high-resolution mode)
    i2c_master_write_byte(cmd, 0x10, true);
    // Stop the I2C communication
    i2c_master_stop(cmd);

    i2c_master_cmd_begin(I2C_NUM_0, cmd, 100 / portTICK_PERIOD_MS);
}

/**
 * @brief task for continuously reading illuminance from the BH1750 every second
 * 
 * @param pvParameters additonal(if there are) parameters of the task function
 */
void bh1750_task(void *pvParameters)
{
    uint8_t data_high, data_low;
    enable_bh1750();
    while(1){
        // create a command to read data
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (BH1750_I2C_ADDRESS << 1) | I2C_MASTER_READ, true);

        i2c_master_read_byte(cmd, &data_high, I2C_MASTER_ACK);
        i2c_master_read_byte(cmd, &data_low, I2C_MASTER_NACK);
        i2c_master_stop(cmd);

        // send the command to read data from the BH1750
        i2c_master_cmd_begin(I2C_NUM_0, cmd, 100 / portTICK_PERIOD_MS);
        i2c_cmd_link_delete(cmd);

        // combine the high and low bytes to get the illuminance in lux
        lux = ((data_high << 8) | data_low)/ 1.2;

        // printf("Illuminance: %d lux\n", lux);
        // Adjust the delay to control the reading frequency (every 1 second)
        vTaskDelay(pdMS_TO_TICKS(1000));
    } 
}


/**
 * @brief initialize non volatile storage to store data passed by user 
 * 
 */
void init_nvs()
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // nvs partition was truncated and needs to be erased
        ESP_ERROR_CHECK(nvs_flash_erase());
        // retry nvs_flash_init
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);
}


/**
 * @brief write passed value of lux by a user to the NVS
 * 
 * @param user_lux_inp contains the value of lux 
 */
void write_to_nvs(uint16_t *user_lux_inp)
{   
    //open NVS handler
    nvs_handle_t handler;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &handler);

    // error while openning handler
    if (err != ESP_OK) {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    }

    // write value to the memory only when it's not the default value, i.e. there is value  passed by a user
    if (*user_lux_inp != 65535){
        nvs_set_i16(handler, "user_lux_inp", *user_lux_inp);
        nvs_commit(handler);
    }
    // close NVS handler
    nvs_close(handler);
}


/**
 * @brief read stored value of lux from the NVS
 * 
 * @param user_lux_inp variable to store value of lux from the NVS
 */
void read_from_nvs(uint16_t *user_lux_inp)
{
    //open NVS handler
    nvs_handle_t handler;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &handler);

    // error while openning handler
    if (err != ESP_OK) {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    }

    // read value from memory 
    err = nvs_get_i16(handler, "user_lux_inp", (int16_t*)user_lux_inp);
    
    // error while reading handler
    if (err != ESP_OK) {
        printf("Error (%s) reading data from the NVS\n", esp_err_to_name(err));
    }
    nvs_close(handler);
}


/**
 * @brief linearize the value of inlluminance(lux) from bh1750 with the duty of LED.
 * Duty corresponds to the ratio of "on" time in that led is turned on 
 * over the whole time of a periodic signal.
 * 
 * @param duty 
 * @param user_lux_inp 
 */
void linearize_value(int *duty, uint16_t user_lux_inp)
{
    float max_lux = (float)user_lux_inp;
    // set max duty to 50% of the duty_resolution. (2 ** 13) * 50% = 4096
    float duty_max = 4096.0; 
    *duty = (float)((max_lux - (float)lux)/max_lux) * duty_max;   
}


/**
 * @brief configure UART to get the value of lux from a user
 */
void uart_cfg()
{
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
    };
    uart_param_config(UART_NUM_0, &uart_config);
    uart_driver_install(UART_NUM_0, BUF_SIZE*2, 0, 0, NULL, 0);
}


/**
 * @brief read data from a terminal passed by user and write/read them to/from NVS
 * 
 * @param data_buffer buffer to save read value as a string
 * @param user_lux_inp variable to save to/read from passed/actual value of the upper intensity limit
 */
void input_lux_value(char *data_buffer, uint16_t *user_lux_inp)
{
    // read data from the terminal input
    int len = uart_read_bytes(UART_NUM_0, (uint8_t*)data_buffer, BUF_SIZE, 400 / portTICK_PERIOD_MS);
    int received_num;
    
    // if there is passed data
    if(len >0){
        data_buffer[len] = '\0';
        // convert the received string to an integer and write it to NVS
        if (sscanf(data_buffer, "%d", &received_num) == 1) {
            *user_lux_inp = (uint16_t)received_num;
            write_to_nvs(user_lux_inp);
        }
        else {
            printf("Warning: Invalid input was passed.\n");
        }
    }
    // read actual value of the high lux boundary from NVS
    read_from_nvs(user_lux_inp);
}


/**
 * @brief change duty of the LED depending on hystersis
 * 
 * @param hystersis value of the hystersis band
 * @param user_lux_inp actual value of the upper intensity limit
 * @param duty actual duty value of the LED
 */
void hystersis_led_set(uint16_t hystersis, uint16_t user_lux_inp, int *duty)
{
    // if actual intesity is higher than  upper intensity limit + hystersis -> set duty to 0
    if (lux > (hystersis + user_lux_inp)){
        *duty = 0;
        ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, *duty));
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0));
    }
    else if(lux < (user_lux_inp - hystersis)){
        ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, *duty));
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0));
    }
}


/**
 * @brief handle wifi events
 * 
 * @param arg 
 * @param event_base 
 * @param event_id 
 * @param event_data 
 */
static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if (event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_id == WIFI_EVENT_STA_CONNECTED) {
        ESP_LOGI(TAG, "Connected to the external hotspot");
    } else if (event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGW(TAG, "Disconnected from the external hotspot");
    }
}


/**
 * @brief configure wifi,register handler and try to start connection
 * 
 */
void wifi_init()
{
    // initialize the TCP/IP stack.
    ESP_ERROR_CHECK(esp_netif_init());

    // create the default event loop.
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // create a default WiFi interface.
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler, NULL, NULL));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = "HONOR",
            .password = "123kuku123",
        }
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
}


/**
 * @brief publish actual value of the intensity from BH1750 to the broker 
 * 
 * @param pvParameters 
 */
void publish_data(void *pvParameters) {
    esp_mqtt_client_handle_t client = (esp_mqtt_client_handle_t)pvParameters;
    char buffer[10];
    while (1) {
        sprintf(buffer, "%d", lux);
        esp_mqtt_client_publish(client, "xalaka00/bh1750/light",buffer, 0, 1, 0);
        //celay for a period of time (e.g., 0,5 seconds)
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}


/**
 * @brief handle mqtt events
 * 
 * @param handler_args 
 * @param base 
 * @param event_id 
 * @param event_data 
 */
void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    esp_mqtt_event_handle_t mqtt_event = event_data;

    switch (event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "Connected to MQTT broker");
            //create task to continuously send data to the broker while broker is connected
            xTaskCreate(publish_data, "publish_data", BUF_SIZE*2, (void *)mqtt_event->client, 5, NULL);
            break;
       
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGW(TAG, "Disconnected from MQTT broker");
            break;
      
        default:
            break;
    }
}


/**
 * @brief configure mqtt broker's address,port and try to connect to the broker
 * 
 */
void start_mqtt()
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = "mqtt://broker.emqx.io:1883",
        .broker.address.port = 1883,
        .credentials.username="12345",
        .credentials.authentication.password="12345",
    };
    esp_mqtt_client_handle_t mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(mqtt_client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(mqtt_client);
}


/**
 * @brief main function 
 * 
 */
void app_main()
{
    int duty = 0; // intitial value of the duty
    uint16_t user_lux_inp = 65535; // initial value of the upper lux limit
    char data_buffer[BUF_SIZE]; // Buffer for received data
    uint16_t hystersis = 20;

    // set the LEDC peripheral configuration
    ledc_init();
    // initialize I2C bus
    init_i2c_bus();
    // intialize NVS
    init_nvs();
    // configure UART
    uart_cfg();

    // configure and start wifi connection
    wifi_init();
    // start mqqt connection
    start_mqtt();

    // crate task to continuously get actual value of illuminance from BH1750
    xTaskCreate(bh1750_task, "bh1750_task", BUF_SIZE*2, NULL, 5, NULL);

    //main cycle 
    while(1){
        // enables reading from a terminal
        input_lux_value(data_buffer,&user_lux_inp);
        // linearize actual lux value
        linearize_value(&duty,user_lux_inp);
        // set value of the duty depending of hystersis
        hystersis_led_set(hystersis,user_lux_inp,&duty);
        // print passed value of the illuminance by a user
        printf("USER INP: %d\n",user_lux_inp);
        // wait 0.5 seconds before the next iteration
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
