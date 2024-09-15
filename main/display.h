#include <esp_err.h>

#include "i2c.h"

#ifndef DISPLAY_H
#define DISPLAY_H

#define CONFIG_I2C_DISPLAY_ADDRESS   0x3C                        // I2C Display slave address
#define CONFIG_I2C_DISPLAY_FREQ_HZ   CONFIG_I2C_MASTER_FREQ_HZ   // I2C master clock frequency

void display_init(i2c_master_bus_handle_t i2c_bus_handle);
void display_info(char *str);
void display_sensor(float temperature, float humidity);
void display_error(uint8_t retry, esp_err_t err);

#endif
