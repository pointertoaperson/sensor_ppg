# Real-Time PPG Heart Rate Monitoring System 

## Project Overview

This system is a complete solution for real-time heart rate monitoring using PPG (Photoplethysmography) signals. It includes signal acquisition, processing, visualization, and wireless transmission.  

## Background and report
[View Project (PDF)](ProjectReport.pdf)

**Key Features:**

- Measures PPG (Photoplethysmography) signals  
- Filters noise using a combination of analog and digital techniques  
- Applies FFT-based frequency estimation and spectral interpolation for accurate heart rate calculation  
- Computes heart rate in real-time  
- Displays output locally on an OLED screen  
- Publishes data wirelessly via MQTT/MQTT-TLS  
- Visualizes the result on a custom Android app  

Originally implemented with **STM32 + ESP8266**, the system has been fully migrated to a **single ESP32**, simplifying the architecture, reducing latency, and improving overall performance.

## Hardware

- ESP32 microcontroller  
- PPG sensor (e.g., MAX30100/MAX30102)  
- OLED display (e.g., SSD1306)  
- Optional: analog filtering components  

## Software

- ESP-IDF / Arduino framework for ESP32  
- MQTT client library for wireless data transmission  
- FFT library for frequency estimation  
- Custom Android app for data visualization  

## Installation

1. Clone the repository inside your esp-idf example folder:  
   ```bash
   git clone https://github.com/pointertoaperson/sensor_ppg.git


