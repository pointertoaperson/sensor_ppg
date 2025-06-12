idf_component_register(SRCS "main.c"
                            "dvr/comm_spi.c"
                            "dvr/display.c" 
                            "dvr/i2c_helper.c"
                       INCLUDE_DIRS "." 
                                    "dvr" 
                                    "network"
                       REQUIRES esp-mqtt)
