idf_component_register(SRCS "LSM303DLHC.cpp"
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES driver esp_timer
                       REQUIRES I2Cdev)
