#include "mqtt.h"
#include "esp_log.h"
#include "mqtt_client.h"
#include "driver/gpio.h"

static const char *TAG = "mqtt";
esp_mqtt_client_handle_t mqtt_client = NULL;

#define MQTT_BROKER_URI      CONFIG_MQTT_BROKER_URI
#define RELAY_GPIO 5
#define STATE_TOPIC  "fireplace/status"
#define CONTROL_TOPIC "fireplace/control"
/**
 * @brief MQTT event handler
 */
static void mqtt_event_handler(void *handler_args, esp_event_base_t base,
                                int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;
    switch ((esp_mqtt_event_id_t)event_id)
    {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "Connected to MQTT broker");

        // Subscribe to control topic
        esp_mqtt_client_subscribe(mqtt_client, CONTROL_TOPIC, 1);
        ESP_LOGI(TAG, "Subscribed to fireplace/control");

        // Publish boot state (retain = 1)
        esp_mqtt_client_publish(mqtt_client, STATE_TOPIC, "BOOT", 0, 1, 1);
        ESP_LOGI(TAG, "Published BOOT state");
        
        break;

    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGW(TAG, "Disconnected from MQTT broker");
        break;

    case MQTT_EVENT_DATA:
    {
        ESP_LOGI(TAG, "Received topic: %s, message: %s", event->topic, event->data);

        // --- Handle control commands ---
        if (strncmp(event->topic, CONTROL_TOPIC, event->topic_len) == 0)
        {
            if (strncmp(event->data, "on", event->data_len) == 0)
            {
                gpio_set_level(RELAY_GPIO, 1);
                esp_mqtt_client_publish(mqtt_client, "fireplace/status", "Relay ON", 0, 1, 1);
                ESP_LOGI(TAG, "Relay ON");
                vTaskDelay(pdMS_TO_TICKS(3000));
                gpio_set_level(RELAY_GPIO, 0);
                esp_mqtt_client_publish(mqtt_client, "fireplace/status", "Relay OFF - Fireplace is turning on", 0, 1, 1);
                ESP_LOGI(TAG, "Relay OFF after 3s - Turn-on fireplace");
            }
            else if (strncmp(event->data, "off", event->data_len) == 0)
            {
                gpio_set_level(RELAY_GPIO, 1);
                esp_mqtt_client_publish(mqtt_client, "fireplace/status", "Relay ON", 0, 1, 1);
                ESP_LOGI(TAG, "Relay ON");
                vTaskDelay(pdMS_TO_TICKS(5000));
                gpio_set_level(RELAY_GPIO, 0);
                esp_mqtt_client_publish(mqtt_client, "fireplace/status", "Relay OFF - Fireplace is turning off", 0, 1, 1);
                ESP_LOGI(TAG, "Relay OFF after 5s - Turn-off fireplace");

            }
            else
            {
                ESP_LOGW(TAG, "Unknown command: %s", event->data);
            }
        }
        break;
    }


    default:
        break;
    }
}

/**
 * @brief Initialize and start MQTT client
 */
void mqtt_start(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = MQTT_BROKER_URI,
    };

    mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
    if (mqtt_client == NULL)
    {
        ESP_LOGE(TAG, "Failed to initialize MQTT client");
        return;
    }

    esp_mqtt_client_register_event(mqtt_client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(mqtt_client);

    ESP_LOGI(TAG, "MQTT client started (URI: %s)", MQTT_BROKER_URI);
}
