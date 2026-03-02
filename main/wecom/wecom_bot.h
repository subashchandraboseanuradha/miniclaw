#pragma once

#include "esp_err.h"
#include <stdbool.h>

/**
 * Initialize WeCom webhook config (build-time defaults + NVS override).
 */
esp_err_t wecom_bot_init(void);

/**
 * Save WeCom webhook URL to NVS.
 */
esp_err_t wecom_set_webhook(const char *url);

/**
 * Send a text message via WeCom robot webhook.
 */
esp_err_t wecom_send_message(const char *text);

/**
 * Returns true if webhook is configured.
 */
bool wecom_is_configured(void);
