idf_component_register(SRCS "main.cpp" "wifi_manager.cpp"
                       INCLUDE_DIRS "."
                       REQUIRES lvgl esp_driver_spi esp_driver_gpio esp_driver_i2c esp_wifi mqtt nvs_flash freertos wifi_provisioning)
