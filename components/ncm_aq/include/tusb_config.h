#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "sdkconfig.h"

// Opcional: define si usas memoria alineada para TinyUSB
#define CFG_TUSB_MEM_SECTION TU_ATTR_ALIGNED(4) DRAM_ATTR

// Buffers de CDC si usas CDC, puedes personalizarlos o eliminarlos
#define CFG_TUD_CDC_RX_BUFSIZE (512)
#define CFG_TUD_CDC_TX_BUFSIZE (512)

#ifdef __cplusplus
}
#endif
