#include <esp_err.h>

#include "i2c.h"

#define I2C_DISPLAY_ADDRESS   0x3C                 // I2C Display slave address
#define I2C_DISPLAY_FREQ_HZ   I2C_MASTER_FREQ_HZ   // I2C master clock frequency

esp_err_t display_init(i2c_master_bus_handle_t i2c_bus_handle);
void display_temperature(float temperature);
void display_humidity(float humidity);
void display_error(uint8_t retry, esp_err_t err);
