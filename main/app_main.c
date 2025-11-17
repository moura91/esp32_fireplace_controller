#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "wifi.h"
#include "mqtt.h"

#define RELAY_GPIO 5

static const char *TAG = "APP";

void app_main(void)
{

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


    // Connect to WiFi
    ESP_ERROR_CHECK(wifi_init());  

    // Start MQTT
    ESP_ERROR_CHECK(mqtt_app_start());

    for (;;) {        
        vTaskDelay(portMAX_DELAY);
    }
}


