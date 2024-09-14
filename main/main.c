#include <stdio.h>
#include <string.h>
#include <driver/gpio.h>
#include <driver/i2c_master.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <esp_system.h>
#include <esp_log.h>

#include <sht4x.h>

#include "i2c.h"
#include "display.h"

static const char *TAG = "app_main";

i2c_master_bus_handle_t i2c_mbus = NULL;

sht4x_t term;

void app_main(void)
{
	ESP_ERROR_CHECK(i2c_master_init(&i2c_mbus));
	ESP_ERROR_CHECK(display_init(i2c_mbus));
	ESP_ERROR_CHECK(sht4x_init(&term, i2c_mbus, SHT40_I2C_ADDR_44));

	int backoff = 1000;
	uint8_t failover = 3;
	float temperature = 0.0f;
	float humidity = 0.0f;
	while (true) {
		esp_err_t err = sht4x_measure_high_precision(&term, &temperature, &humidity);
		if (err != ESP_OK) {
			ESP_LOGE(TAG, "Failed to read from SHT4X sensor (left %d, code %d)", failover, err);
			display_error(failover, err);

			if (--failover == 0) {
				ESP_LOGE(TAG, "Unable to recover, shutting down");
				break;
			} else {
				// ESP_LOGD(TAG, "Backing off to %d ms", backoff);
				backoff = backoff << 2;
			}
		}
		else {
			display_temperature(temperature);
			backoff = 1000;
		}

		vTaskDelay(pdMS_TO_TICKS(backoff));
	}
}