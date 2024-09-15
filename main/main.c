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
#include "storage.h"
#include "wifi.h"
#include "http.h"
#include "sensor.h"

static const char *TAG = "app_main";

static i2c_master_bus_handle_t i2c_mbus = NULL;

static sensor_t thermostat;

void app_main(void)
{
	i2c_master_init(&i2c_mbus);
	display_init(i2c_mbus);
	ESP_ERROR_CHECK(sht4x_init(&thermostat.sensor, i2c_mbus, SHT40_I2C_ADDR_44));
	storage_init();
	wifi_sta_init();
	http_init(&thermostat);

	int backoff = 1000;
	uint8_t failover = 3;
	thermostat.temperature = 0.0f;
	thermostat.humidity = 0.0f;
	while (true) {
		esp_err_t err = sht4x_measure_high_precision(&thermostat.sensor, &thermostat.temperature, &thermostat.humidity);
		if (err != ESP_OK) {
			failover = failover - 1;
			ESP_LOGE(TAG, "Failed to read from SHT4X sensor (left %d, code %d)", failover, err);
			display_error(failover, err);
			thermostat.err = err;

			if (failover == 0) {
				ESP_LOGE(TAG, "Unable to recover, shutting down");
				break;
			} else {
				backoff = backoff << 1;
				ESP_LOGD(TAG, "Backing off with %d ms", backoff);
			}
		}
		else {
			display_sensor(thermostat.temperature, thermostat.humidity);
			thermostat.err = ESP_OK;
			backoff = 1000;
		}

		vTaskDelay(pdMS_TO_TICKS(backoff));
	}
}
