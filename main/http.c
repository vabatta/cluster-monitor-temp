#include <esp_http_server.h>
#include <esp_event.h>
#include <esp_netif.h>
#include <esp_log.h>
#include <esp_err.h>
#include <esp_wifi.h>

#include "sensor.h"

static const char *TAG = "http";

static sensor_t *psensor = NULL;

esp_err_t get_handler(httpd_req_t *req)
{
	char json_buffer[36];
	if (psensor->err == ESP_OK) {
		sprintf(json_buffer, "{\"temperature\": %2.0f, \"humidity\": %2.0f}", psensor->temperature, psensor->humidity);
	}
	else {
		sprintf(json_buffer, "{\"error\": %d}", psensor->err);
	}

	httpd_resp_set_status(req, HTTPD_200);
	httpd_resp_set_type(req, HTTPD_TYPE_JSON);
	httpd_resp_send(req, json_buffer, HTTPD_RESP_USE_STRLEN);
	return ESP_OK;
}

/* URI handler structure for GET /uri */
httpd_uri_t uri_get = {
	.uri      = "/",
	.method   = HTTP_GET,
	.handler  = get_handler,
	.user_ctx = NULL
};

/* Function for starting the webserver */
httpd_handle_t start_webserver(void)
{
	httpd_config_t config = HTTPD_DEFAULT_CONFIG();
	// Setting port as 8001 when building for Linux. Port 80 can be used only by a privileged user in linux.
	// So when a unprivileged user tries to run the application, it throws bind error and the server is not started.
	// Port 8001 can be used by an unprivileged user as well. So the application will not throw bind error and the
	// server will be started.
	config.server_port = 8001;

	httpd_handle_t new_server = NULL;

	/* Start the httpd server */
	ESP_LOGI(TAG, "Binding webserver on port %d...", config.server_port);
	esp_err_t err = httpd_start(&new_server, &config);
	if (err == ESP_OK) {
		ESP_LOGI(TAG, "Webserver booted");
		/* Register URI handlers */
		httpd_register_uri_handler(new_server, &uri_get);
	}
	else {
		ESP_LOGE(TAG, "Failed to boot webserver (code %d)", err);
	}
	/* If server failed to start, handle will be NULL */
	return new_server;
}

esp_err_t stop_webserver(httpd_handle_t server)
{
	return httpd_stop(server);
}

static void disconnect_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
	httpd_handle_t* server = (httpd_handle_t*) arg;
	ESP_LOGI(TAG, "Stopping webserver...");
	if (*server) {
		if (stop_webserver(*server) == ESP_OK) {
			ESP_LOGI(TAG, "Webserver stopped");
			*server = NULL;
		}
		else {
			ESP_LOGE(TAG, "Failed to stop webserver");
		}
	}
	else {
		ESP_LOGE(TAG, "Webserver is already stopped");
	}
}

static void connect_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
	ESP_LOGI(TAG, "Booting webserver...");
	httpd_handle_t server = (httpd_handle_t *) arg;
	if (server == NULL) {
		server = start_webserver();
	}
	else {
		ESP_LOGE(TAG, "Webserver is already running");
	}
}

void http_init(sensor_t *psensor_in) {
	psensor = psensor_in;

	httpd_handle_t server = NULL;
	ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &connect_handler, &server));
	ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &disconnect_handler, &server));

	server = start_webserver();
}
