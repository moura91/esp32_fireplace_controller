idf_component_register(
    SRCS "app_main.c" "wifi.c" "mqtt.c"
    INCLUDE_DIRS "."
    REQUIRES freertos esp_driver_gpio esp_netif esp_wifi nvs_flash esp_event mqtt
)

