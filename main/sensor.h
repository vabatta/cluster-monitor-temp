#include <esp_err.h>

#include <sht4x.h>

#ifndef SENSOR_H
#define SENSOR_H

typedef struct {
	sht4x_t sensor;
	float temperature;
	float humidity;
	esp_err_t err;
} sensor_t;

#endif