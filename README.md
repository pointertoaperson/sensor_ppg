# connectHeart — STM32, ESP8266 Hardware Bring-Up & CMSIS Integration

## Overview

**connectHeart** is an embedded systems/IoT project to evaluate HEART RATE and OXYGEN saturation from PPG signal (from custom built analog circuit) based on the STM32F103 microcontroller for signal processing and the ESP8266 as a display controller and radio unit. This is the initial development phase focused on **hardware bring-up** and **integration of the CMSIS library** for low-level peripheral control. The goal is to build a reliable foundation for embedded development using modular, reusable drivers and communication interfaces.

This stage includes interfacing key components, validating hardware, and building minimal firmware drivers.

---

## Current Components & Functionality

- **CMSIS-Based STM32 Drivers**  
  Initialization and control of core peripherals using CMSIS libraries.

- **DMA and Timer-Enabled ADC**  
  Enables signal sampling with controllable frequency and streamlined data conversion on STM32.

- **SSD1306 OLED Display**  
  Controlled via **I2C**, with display logic implemented on the ESP8266.

- **SPI Interface**  
  A wrapper module supports **ESP8266** as an **SPI slave** and STM32 as the **SPI master**, enabling future master-to-slave data communication.

- **I2C Communication**  
  ESP8266 I2C helper layer for streamlined communication with peripherals like SSD1306.

- **Delay Utility**  
  CPU-based and timer-based delay functions abstracted for portability across platforms.

- **Wifi & MQTT**
  Wifi credential saving and reset in NVS flash and MQTT connection. 

- **LED & ADC**
  Data acquisition process implemented.
---

## TO DO
- Implement PPG and SPO2 algorithm.

- Implement security on netwrok.

- Build a basic android app interface.

## Status

> ⚠️ **This project is in the hardware bring-up and early testing phase.**

- Basic drivers are implemented and tested individually.
- CMSIS-DSP integration is functional.
- Communication between **STM32** and **ESP8266**, and with **SSD1306**, is working.
- Modules are actively being evaluated and may be restructured or optimized in future phases.

---

## License

This project is licensed under the **MIT License**.  

Note:
This project includes third-party components (e.g., CMSIS, STM32F103 startup and linker etc.)under their respective licenses. License information, if provided by the original authors, is included in their respective files.

© 2025 Umesh Puri
