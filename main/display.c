#include <stdio.h>
#include <string.h>
#include <driver/i2c_master.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <esp_system.h>
#include <esp_log.h>

#include <u8g2.h>

#include "display.h"

static const char *TAG = "display_ctl";

i2c_master_dev_handle_t i2c_display_device = NULL;

u8g2_t u8g2;

uint8_t u8x8_gpio_and_delay_esp32(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
	switch (msg) {
		case U8X8_MSG_GPIO_AND_DELAY_INIT:
			break;
		case U8X8_MSG_DELAY_MILLI:
			vTaskDelay(pdMS_TO_TICKS(arg_int));
			break;
		case U8X8_MSG_DELAY_10MICRO:
			vTaskDelay(pdMS_TO_TICKS(0.01 * arg_int));
			break;
		case U8X8_MSG_DELAY_100NANO:
			vTaskDelay(pdMS_TO_TICKS(0.0001));
			break;
		default:
			return 0;
	}
	return 1;
}


uint8_t u8x8_byte_esp32_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
	static uint8_t buffer[32];
	static uint8_t buf_idx;
	uint8_t *data;

	switch (msg) {
		case U8X8_MSG_BYTE_SEND:
			data = (uint8_t *)arg_ptr;
			while (arg_int > 0) {
				buffer[buf_idx++] = *data;
				data++;
				arg_int--;
			}
			break;
		case U8X8_MSG_BYTE_INIT:
			// Already initialized in i2c_master_init()
			break;
		case U8X8_MSG_BYTE_SET_DC:
			// DC (Data/Command) bit is set as part of the I2C data
			break;
		case U8X8_MSG_BYTE_START_TRANSFER:
			buf_idx = 0;
			break;
		case U8X8_MSG_BYTE_END_TRANSFER:
			i2c_master_transmit(i2c_display_device, buffer, buf_idx, 1000 / portTICK_PERIOD_MS);
			break;
		default:
			return 0;
			break;
	}
	return 1;
}

esp_err_t display_init(i2c_master_bus_handle_t i2c_bus_handle) {
	ESP_LOGI(TAG, "Initialising I2C display...");
	i2c_device_config_t i2c_dev_config = {
		.dev_addr_length = I2C_ADDR_BIT_LEN_7,
		.device_address = I2C_DISPLAY_ADDRESS,
		.scl_speed_hz = I2C_DISPLAY_FREQ_HZ,
		.scl_wait_us = 1000,
		.flags = {
			.disable_ack_check = false
		}
	};

	esp_err_t err = i2c_master_bus_add_device(i2c_bus_handle, &i2c_dev_config, &i2c_display_device);
	if (err != ESP_OK) {
		ESP_LOGE(TAG, "Failed to add display to I2C bus (code %d)", err);
		return err;
	}

	u8g2_Setup_ssd1306_i2c_128x32_univision_f(&u8g2, U8G2_R0, u8x8_byte_esp32_i2c, u8x8_gpio_and_delay_esp32);
	u8g2_InitDisplay(&u8g2);

	vTaskDelay(pdMS_TO_TICKS(100));
	u8g2_SetPowerSave(&u8g2, 0);  // Wake up display

	// x: 30 y: 14 with 8 height | y: 24 with 16 
	u8g2_ClearBuffer(&u8g2);
	u8g2_SetFont(&u8g2, u8g2_font_inb16_mr);
	u8g2_DrawStr(&u8g2, 30, 24, "boot");
	u8g2_SendBuffer(&u8g2);

	ESP_LOGI(TAG, "I2C Display booted");
	return ESP_OK;
}

static char number_str[10];

void display_temperature(float temperature) {
	u8g2_ClearBuffer(&u8g2);
	u8g2_SetFont(&u8g2, u8g2_font_inb16_mr);
	sprintf(number_str, "%2.0f", temperature);
	u8g2_DrawStr(&u8g2, 28 + 0, 24, number_str);
	u8g2_SendBuffer(&u8g2);
}

void display_humidity(float humidity) {
	u8g2_ClearBuffer(&u8g2);
	u8g2_SetFont(&u8g2, u8g2_font_inb16_mr);
	sprintf(number_str, "%2.0f", humidity);
	u8g2_DrawStr(&u8g2, 28 + 0, 24, number_str);
	u8g2_SendBuffer(&u8g2);
}

void display_error(uint8_t retry, esp_err_t err) {
	u8g2_ClearBuffer(&u8g2);
	u8g2_SetFont(&u8g2, u8g2_font_inb16_mr);

	switch (retry) {
		case 2:
			sprintf(number_str, "!!%d", err);
			break;
		case 1:
			sprintf(number_str, "!!!%d", err);
			break;
		case 0:
			sprintf(number_str, "ERR%d", err);
			break;
		default:
			sprintf(number_str, "!%d", err);
	}

	u8g2_DrawStr(&u8g2, 28, 24, number_str);
	u8g2_SendBuffer(&u8g2);
}
