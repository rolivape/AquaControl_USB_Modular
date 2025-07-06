#include "system_aq.h"
#include "nvs_init_aq.h"
#include "bios_config_aq.h"
#include "bios_config_aq_menu.h"

void system_init_aq(void)
{
    nvs_init_aq();
    esp_err_t bios_config_init_aq(); // 🔄 Debería estar aquí
    bios_config_menu_aq();           // 🛠️ Para probar la entrada UART
    // esp_err_t bios_config_check_or_menu_aq(); // 👈 encapsula todo
}
