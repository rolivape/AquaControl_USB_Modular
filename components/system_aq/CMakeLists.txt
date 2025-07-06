idf_component_register(
    SRCS "system_aq.c"
    INCLUDE_DIRS "include"
    PRIV_REQUIRES nvs_init_aq bios_config_aq
)
