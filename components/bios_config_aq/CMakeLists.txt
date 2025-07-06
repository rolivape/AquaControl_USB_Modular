idf_component_register(
    SRCS "bios_config_aq.c"
    INCLUDE_DIRS "include"
    PRIV_REQUIRES nvs_flash driver
)
