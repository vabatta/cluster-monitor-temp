#include <esp_err.h>

#define I2C_MASTER_SCL_IO           GPIO_NUM_6   // GPIO number for I2C master clock
#define I2C_MASTER_SDA_IO           GPIO_NUM_5   // GPIO number for I2C master data
#define I2C_MASTER_NUM              I2C_NUM_0    // I2C port number for master dev
#define I2C_MASTER_FREQ_HZ          400000       // I2C master clock frequency
#define I2C_MASTER_TX_BUF_DISABLE   0            // I2C master doesn't need buffer
#define I2C_MASTER_RX_BUF_DISABLE   0            // I2C master doesn't need buffer

esp_err_t i2c_master_init(i2c_master_bus_handle_t *pi2c_mbus);
