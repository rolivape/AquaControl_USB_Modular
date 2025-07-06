idf_component_register(
    SRCS "ncm_aq.c"
    INCLUDE_DIRS "include"
    REQUIRES esp_tinyusb
    PRIV_REQUIRES tinyusb
)
