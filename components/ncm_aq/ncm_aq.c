#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "tinyusb.h"
#include "tinyusb_net.h"
#include "ncm_aq.h"

static const char *TAG = "NCM_AQ";

// Firma correcta según tinyusb_net_config_t
static esp_err_t on_data_rx_cb(void *buffer, uint16_t len, void *buff_free_arg)
{
    ESP_LOGI(TAG, "Recibidos %u bytes por NCM", len);
    // Aquí puedes hacer: parseo, envío a cola, etc.
    return ESP_OK;
}

void ncm_init_aq(void)
{
    ESP_LOGI(TAG, "Instalando driver TinyUSB...");

    const tinyusb_config_t tusb_cfg = {
        .device_descriptor = NULL,
        .string_descriptor = NULL,
        .external_phy = false};
    ESP_ERROR_CHECK(tinyusb_driver_install(&tusb_cfg));

    ESP_LOGI(TAG, "Inicializando red USB NCM...");

    tinyusb_net_config_t net_cfg = {
        .on_recv_callback = on_data_rx_cb,
        .on_init_callback = NULL,
        .free_tx_buffer = NULL};

    ESP_ERROR_CHECK(tinyusb_net_init(TINYUSB_USBDEV_0, &net_cfg));
    ESP_LOGI(TAG, "Red USB NCM inicializada correctamente.");
}
