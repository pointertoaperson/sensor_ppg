
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


#if 0

void i2c_init(void)
{
    int i2c_master_port = I2C_MASTER_NUM;

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 100000 // Set clock speed, e.g., 100kHz
    };

    // First configure, then install
    i2c_param_config(i2c_master_port, &conf);
    i2c_driver_install(i2c_master_port, conf.mode, 0, 0, 0);
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
	i2c_master_cmd_begin(I2C_NUM_0, cmd, pdMS_TO_TICKS(10));

	i2c_cmd_link_delete(cmd);
}
#else

static const char *TAG = "i2c_helper";
void i2c_init(void)
{
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };
    i2c_param_config(I2C_MASTER_NUM, &conf);
    i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);
}

void i2c_write(uint8_t data)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    esp_err_t err;

    i2c_master_start(cmd);

    // Write OLED address with write bit (0)
    i2c_master_write_byte(cmd, (OLED_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);

    // Send control bytes and command/data bytes per SSD1306 protocol
    //i2c_master_write_byte(cmd, 0x80, true); // Co = 1, D/C# = 0 -> Command
    //i2c_master_write_byte(cmd, 0xB0, true); // Page address command (example)
    i2c_master_write_byte(cmd, 0x40, true); // Control byte for data

    i2c_master_write_byte(cmd, data, true);

    i2c_master_stop(cmd);

    err = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, pdMS_TO_TICKS(10));
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "I2C write failed: %s", esp_err_to_name(err));
    }

    i2c_cmd_link_delete(cmd);
}

#endif