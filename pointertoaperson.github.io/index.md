---
layout: default
title: Full Project Report
---

# **Real-Time IoT-Enabled Cardiovascular Monitoring System**  

**Authors:**  Umesh Puri  
  

---

# **Contents**

1. Introduction  
2. Background and System Overview  
3. Materials and Methods  
4. Workflow and Development Process  
5. Results and Observations  
6. Review of Results  
7. Summary  
8. References  

---

# **List of Abbreviations**

- **AFE:** Analog Front End  
- **FFT:** Fast Fourier Transform  
- **MQTT:** Message Queuing Telemetry Transport  
- **SoC:** System on Chip  
- **FreeRTOS:** Free Real-Time Operating System  

---

# **1. Introduction**

This report discusses the development of a real-time heart rate monitoring system based on the Photoplethysmography (PPG) principle.  
The project builds upon a PPG sensor prototype developed earlier in a sensor course and extends it into a complete digital + IoT monitoring system.

The goal was to make a **high-precision, low-latency** cardiovascular feedback system suitable for:

- Medical diagnostics  
- Sports monitoring  
- General wellness tracking  

Originally, the system used two microcontrollers:

- **STM32F103C6T6** for sampling + DSP  
- **ESP8266** for communication  

Later, the design was simplified to a **single ESP32**, which handles:

- Data acquisition  
- Processing  
- Display  
- Wi-Fi communication  

### **Project Objectives**

- Implement a reliable real-time PPG-based heart rate monitor  
- Design firmware for acquisition, filtering, and BPM computation  
- Integrate ESP32 with OLED and Wi-Fi  
- Create a modular system for future SPO₂ addition  

---

# **2. Background and System Overview**

## **2.1 Photoplethysmography (PPG) Principle**

PPG detects changes in blood volume using light.  
When light shines on skin:

- More blood → more absorption  
- Less blood → more reflection  

This periodic variation corresponds to heartbeats.

### **Figure 1 — Absorbance of blood by light**  
![Absorbance Placeholder](images/absorbance.jpg)

### **Figure 2 — Typical PPG sensor setup**
![PPG Setup Placeholder](images/ppg_setup.jpg)

---

## **2.2 System Overview**

The system:

1. Acquires raw analog PPG signal  
2. Samples via ESP32 ADC  
3. Applies digital filtering  
4. Detects peaks → BPM  
5. Displays data locally  
6. Publishes via MQTT to Android app  

### **System Block Diagram**
![System Overview Placeholder](images/system_overview.jpg)

---

# **3. Materials and Methods**

## **3.1 Hardware Design**

The system is entirely built on **ESP32**, which performs:

- Data acquisition  
- Filtering  
- FFT  
- Networking  
- Display updates  

![Hardware Block Placeholder](images/hardware_block.jpg)

---

## **3.2 Hardware Components**

- **ESP32 Microcontroller**
- **PPG Sensor:** IR + Red LED + Photodiode  
- **SSD1306 OLED**
- **Power:** 5V USB  

### **Analog Front End (AFE)**

AFE consists of:

- PIN photodiode  
- Transimpedance amplifier  
- Band-pass filter  
- Gain stage  

### **Figure — Filter Simulation**
![Filter Simulation Placeholder](images/filter_sim.jpg)

### **Figure — AFE Testing (Noisy Signal)**
![AFE Testing Noise Placeholder](images/afe_noise.jpg)

### **Figure — AFE Testing (Cleaned Signal)**
![AFE Testing Clean Placeholder](images/afe_clean.jpg)

### **Figure — AFE Schematic**
![AFE Schematic Placeholder](images/afe_schematic.jpg)

### **Figure — AFE PCB**
![AFE PCB Placeholder](images/afe_pcb.jpg)

---

## **3.3 Digital Backend**

Initially: STM32 + ESP8266  
Final: **ESP32WROOM SoC**

### Provides:

- 12-bit ADC  
- Dual CPU  
- I²C  
- Wi-Fi/Bluetooth  

### **Figure — Digital Backend Schematic**
![Digital Backend Schematic Placeholder](images/digital_backend_schematic.jpg)

### **Figure — Digital Backend PCB**
![Digital Backend PCB Placeholder](images/digital_backend_pcb.jpg)

---

## **3.4 Software and Algorithm Design**

Developed using **ESP-IDF** and **FreeRTOS**.

### **Tasks:**

- **Acquisition Task:** Timer-based ADC sampling  
- **Processing Task:** Digital filtering + peak detection  
- **Display Task:** OLED refresh  
- **Wi-Fi Task:** MQTT publisher  

---

## **3.5 Signal Processing**

2048 samples at 50 Hz were collected for analysis.

### **Raw Collected Signal**
![Raw Signal Placeholder](images/raw_signal.jpg)

### **Zoomed Peak**
![Zoomed Peak Placeholder](images/zoom_peak.jpg)

Noise sources:

- Low-frequency drift  
- High-frequency random noise  
- Motion artifacts  

---

## **3.6 Foundational Principle of Algorithm**

PPG signal is modeled as sum of multiple sinusoids.  
Largest peak corresponds to heartbeat frequency.

Using **DFT**:

\[
X[k] = \frac{1}{N} \sum_{n=0}^{N-1} x[n] e^{-i 2\pi kn/N}
\]

### **PPG + FFT Example**
![FFT Example Placeholder](images/fft_example.jpg)

Real signals → symmetric FFT → use N/2 bins.

---

### **Adaptive Filtering**

1. Remove impossible frequencies (<0.4 Hz or >5 Hz)  
2. Keep only strongest bin  
3. Apply frequency interpolation  

### **Frequency Resolution Issue**

FFT resolution at 50 Hz sampling & 256 samples:

≈ 11.71875 BPM steps  
Too coarse → interpolation required.

---

### **Interpolation Methods Tested**

- Quinn’s Second Estimator  
- **Log-Parabolic Interpolation → chosen**

### **Figures:**
![Comparison Placeholder](images/comparison.jpg)

![Quinn Placeholder](images/quinn.jpg)

![Error Graph Placeholder](images/error_graph.jpg)

![Log Parabolic Placeholder](images/log_parabolic.jpg)

---

## **3.7 Development Tools**

- ESP32WROOM  
- ESP-IDF  
- Python  
- LTspice  
- GitHub  

### **Network Interface**
![Android App Placeholder](images/android_app.jpg)

---

# **4. Workflow and Development Process**

1. Initial dual-MCU prototype  
2. Migration to ESP32  
3. Firmware development  
4. Signal testing  
5. Optimization  

---

# **5. Results and Observations**

- Real-time heart rate monitoring  
- OLED updates ~ every 2.6s  
- Power consumption <150mA  
- Stable under ideal conditions  
- Motion artifacts still present (normal)  

---

# **6. Review of Results**

### Achievements

- Real-time HR monitoring  
- Single-board ESP32 implementation  
- Reliable performance  
- Wireless + display-integrated system  

### Limitations

- No SPO₂ yet  
- Sensitive to strong motion  
- Could improve filtering  

---

# **7. Summary**

The project successfully demonstrates a full real-time PPG heart-rate monitoring system using ESP32.

### Key Contributions

- Fully functional analog + digital design  
- Real-time DSP on microcontroller  
- Secure wireless communication  
- Prototype ready for expansion  

---

# **8. References**

(Add your formatted references here)

---

# **Appendix**
![Appendix Placeholder](images/appendix.jpg)

