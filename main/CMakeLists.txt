idf_component_register(SRCS "main.c" "i2c.c" "display.c" "wifi.c" "storage.c" "http.c" 
                    INCLUDE_DIRS "."
                    REQUIRES u8g2 sht4x esp_driver_i2c esp_driver_gpio nvs_flash esp_wifi esp_http_server)
