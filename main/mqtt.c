#include "mqtt.h"
#include "esp_log.h"
#include "mqtt_client.h"
#include "driver/gpio.h"

static const char *TAG = "mqtt";
static esp_mqtt_client_handle_t mqtt_client = NULL;

#define MQTT_BROKER_URI      CONFIG_MQTT_BROKER_URI
#define RELAY_GPIO 5

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
        esp_mqtt_client_subscribe(mqtt_client, "fireplace/control", 1);
        ESP_LOGI(TAG, "Subscribed to topic: fireplace/control");
        break;

    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGW(TAG, "Disconnected from MQTT broker");
        break;

    case MQTT_EVENT_DATA:
    {
        char topic[event->topic_len + 1];
        char data[event->data_len + 1];
        memcpy(topic, event->topic, event->topic_len);
        memcpy(data, event->data, event->data_len);
        topic[event->topic_len] = '\0';
        data[event->data_len] = '\0';

        ESP_LOGI(TAG, "Received topic: %s, message: %s", topic, data);

        // --- Handle control commands ---
        if (strcmp(topic, "fireplace/control") == 0)
        {
            if (strcasecmp(data, "on") == 0)
            {
                gpio_set_level(RELAY_GPIO, 1);
                ESP_LOGI(TAG, "Fireplace relay ON");
                esp_mqtt_client_publish(mqtt_client, "fireplace/status", "Relay ON", 0, 1, 0);
            }
            else if (strcasecmp(data, "off") == 0)
            {
                gpio_set_level(RELAY_GPIO, 0);
                ESP_LOGI(TAG, "Fireplace relay OFF");
                esp_mqtt_client_publish(mqtt_client, "fireplace/status", "Relay OFF", 0, 1, 0);
            }
            else
            {
                ESP_LOGW(TAG, "Unknown command: %s", data);
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
esp_err_t mqtt_app_start(void)
{
    const esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = MQTT_BROKER_URI,
    };

    mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
    if (mqtt_client == NULL)
    {
        ESP_LOGE(TAG, "Failed to initialize MQTT client");
        return ESP_FAIL;
    }

    esp_mqtt_client_register_event(mqtt_client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    ESP_ERROR_CHECK(esp_mqtt_client_start(mqtt_client));

    ESP_LOGI(TAG, "MQTT client started (URI: %s)", MQTT_BROKER_URI);
    return ESP_OK;
}

/**
 * @brief Publish a message to a topic
 */
esp_err_t mqtt_publish(const char *topic, const char *message)
{
    if (mqtt_client == NULL)
    {
        ESP_LOGE(TAG, "MQTT client not initialized");
        return ESP_FAIL;
    }

    int msg_id = esp_mqtt_client_publish(mqtt_client, topic, message, 0, 1, 0);
    if (msg_id == -1)
    {
        ESP_LOGE(TAG, "Failed to publish message");
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Published message to %s: %s", topic, message);
    return ESP_OK;
}

