#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"

#define RELAY_GPIO 5

static const char *TAG = "APP";

void app_main(void)
{
    gpio_config_t io_conf = {
        .pin_bit_mask = 1ULL << RELAY_GPIO,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = 0,
        .pull_down_en = 0,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);

    gpio_set_level(RELAY_GPIO, 0); //ensure off state at start

    vTaskDelay(pdMS_TO_TICKS(5000)); //wait 5s

    gpio_set_level(RELAY_GPIO, 1); // turn-on relay

    vTaskDelay(pdMS_TO_TICKS(3000)); //wait 3s

    gpio_set_level(RELAY_GPIO, 0); // turn-off relay

    for (;;) {        
        vTaskDelay(portMAX_DELAY);
    }
}


