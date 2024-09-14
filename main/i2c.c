#include <driver/gpio.h>
#include <driver/i2c_master.h>

#include <esp_system.h>
#include <esp_log.h>

#include "i2c.h"

static const char *TAG = "i2c";

esp_err_t i2c_master_init(i2c_master_bus_handle_t *pi2c_mbus) {
	ESP_LOGI(TAG, "Initialising I2C master bus...");
	i2c_master_bus_config_t i2c_bus_config = {
		.i2c_port = -1,
		.sda_io_num = I2C_MASTER_SDA_IO,
		.scl_io_num = I2C_MASTER_SCL_IO,
		.clk_source = I2C_CLK_SRC_DEFAULT,
		.glitch_ignore_cnt = 7,
		.intr_priority = 1,
		.flags = {
			.enable_internal_pullup = true
		}
	};
	esp_err_t err = i2c_new_master_bus(&i2c_bus_config, pi2c_mbus);
	if (err != ESP_OK) {
		ESP_LOGE(TAG, "Failed to initialise I2C master bus (code %d)", err);
	}

	ESP_LOGI(TAG, "I2C master bus booted");
	return err;
}
