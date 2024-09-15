#include <esp_wifi.h>

#ifndef WIFI_H
#define WIFI_H

#define CONFIG_WIFI_SSID            "myssid"
#define CONFIG_WIFI_PASSWORD        "password"
#define CONFIG_WIFI_AUTH_METHOD     WIFI_AUTH_WPA_WPA2_PSK
#define CONFIG_WIFI_RETRY_CONNECT   5

void wifi_sta_init(void);

#endif
