#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "wifi.h"
#include "mqtt.h"
#include "dht.h"
#include "nvs_flash.h"
#include "mqtt_client.h"


#define RELAY_GPIO 5
#define DHT_GPIO 4

static const char *TAG = "APP";


// Sensor task
void sensor_task(void *arg)
{
    while (1) {
        float temperature = 0, humidity = 0;
        esp_err_t result = dht_read_float_data(DHT_TYPE_DHT11, DHT_GPIO, &humidity, &temperature);

        if (result == ESP_OK) {
            ESP_LOGI("SENSOR", "Temp: %.1f°C  Humidity: %.1f%%", temperature, humidity);
            if(mqtt_client){
                char payload[64];
                snprintf(payload, sizeof(payload), "{\"temp\":%.1f,\"hum\":%.1f}", temperature, humidity);
                int msg_id = esp_mqtt_client_publish(mqtt_client, "fireplace/sensor", payload, 0, 1, 0);
                ESP_LOGI("SENSOR", "Published sensor data, msg_id=%d", msg_id);
            }
            
        } else {
            ESP_LOGE("SENSOR", "Failed to read data: %s", esp_err_to_name(result));
        }

        vTaskDelay(pdMS_TO_TICKS(60000)); // read every 1 minute
    }
}

void app_main(void)
{
    nvs_flash_init();

    // Initializa relay GPIO
    gpio_config_t io_conf = {
        .pin_bit_mask = 1ULL << RELAY_GPIO,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = 0,
        .pull_down_en = 0,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);
    gpio_set_level(RELAY_GPIO, 0); //ensure off state at start

    // Initializa DHT11 data GPIO
    gpio_config_t io_sensor_conf = {
        .pin_bit_mask = 1ULL << DHT_GPIO,
        .mode = GPIO_MODE_INPUT,   // start as input
        .pull_down_en = 0,
        .pull_up_en = 0,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_sensor_conf);
    

    ESP_LOGI(TAG, "Initialize Wi-Fi and MQTT");
    // Connect to WiFi
    wifi_init();  

    // Start MQTT
    mqtt_start();

    ESP_LOGI(TAG, "System ready for MQTT remote control");

    // Start sensor task
    xTaskCreate(sensor_task, "sensor_task", 4096, NULL, 5, NULL);

}
