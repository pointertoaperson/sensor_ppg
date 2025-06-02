
/**
 * @file    i2c_helper.c
 * @brief   I2C helper functions for ESP8266 peripheral communication utilities and drivers like SSD1306, sensors, etc.
 * @author  Umesh Puri
 * @date    2025-06-02
 * @version 1.0
 *
 * This file provides utility functions for initializing and communicating
 * with devices over the I2C bus, used by peripherals such as SSD1306 displays,
 * sensors, and communication modules on STM32F103.
 *
 * @license MIT License
 * Copyright (c) 2025 Umesh
 */

#include "i2c_helper.h"

void i2c_init()
{
	int i2c_master_port = I2C_EXAMPLE_MASTER_NUM;
	i2c_config_t conf;
	conf.mode = I2C_MODE_MASTER;
	conf.sda_io_num = I2C_EXAMPLE_MASTER_SDA_IO;
	conf.sda_pullup_en = 0;
	conf.scl_io_num = I2C_EXAMPLE_MASTER_SCL_IO;
	conf.scl_pullup_en = 0;
	i2c_driver_install(i2c_master_port, conf.mode);
	i2c_param_config(i2c_master_port, &conf);
}

void i2c_write(uint8_t data)
{
	i2c_cmd_handle_t cmd;

	cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (OLED_I2C_ADDRESS << 1) | 0, true);
	i2c_master_write_byte(cmd, 0x80, true);
	i2c_master_write_byte(cmd, 0xB0, true);
	i2c_master_write_byte(cmd, 0x40, true);

	i2c_master_write_byte(cmd, data, true);

	i2c_master_stop(cmd);
	i2c_master_cmd_begin(I2C_NUM_0, cmd, 10 / portTICK_PERIOD_MS);
	i2c_cmd_link_delete(cmd);
}
