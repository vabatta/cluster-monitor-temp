#include <driver/i2c_master.h>

#ifndef I2C_H
#define I2C_H

#define CONFIG_I2C_MASTER_SCL_IO    GPIO_NUM_6   // GPIO number for I2C master clock
#define CONFIG_I2C_MASTER_SDA_IO    GPIO_NUM_5   // GPIO number for I2C master data
#define CONFIG_I2C_MASTER_NUM       I2C_NUM_0    // I2C port number for master dev
#define CONFIG_I2C_MASTER_FREQ_HZ   400000       // I2C master clock frequency

void i2c_master_init(i2c_master_bus_handle_t *pi2c_mbus);

#endif
