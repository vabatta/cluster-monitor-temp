idf_component_register(SRCS "main.c" "i2c.c" "display.c" 
                    INCLUDE_DIRS "."
                    REQUIRES u8g2 sht4x esp_driver_i2c esp_driver_gpio)
