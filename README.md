# ESP32 Bioreactor Heating Subsystem (PID)

This project implements a precise temperature control system for a mock bioreactor using an ESP32. It utilizes a PID (Proportional-Integral-Derivative) algorithm to maintain a target temperature by modulating a heating pad (or resistor) via a MOSFET. It includes Steinhart-Hart equation conversion for NTC thermistors and safety cutoffs.

## 🌡️ Features

* **PID Control Loop:** smooths temperature curves and prevents overshoot better than simple On/Off (Bang-Bang) controllers.
* **Precise Sensing:** Implements the Steinhart-Hart equation to convert non-linear NTC thermistor resistance into degrees Celsius.
* **Safety First:** Includes a hard-coded `SAFETY_MAX_TEMP` (35°C) to immediately cut power in case of sensor failure or thermal runaway.
* **Telemetry:** Outputs CSV-style data to the Serial Monitor (Time, Temp, Error, PID terms, Power%) for easy plotting in Excel or Python.
* **Overshoot Protection:** limits the Integral term (Anti-windup) to prevent the system from getting "stuck" at 100% power.

## 🛠️ Hardware Requirements

* **Microcontroller:** ESP32 Development Board.
* **Sensor:** NTC 10k Thermistor (Beta ~3950 or 4220).
* **Resistor:** 10kΩ Resistor (for the Voltage Divider).
* **Actuator:** Logic-Level MOSFET (e.g., IRLZ44N) or Transistor suitable for your heater.
* **Heater:** Polyimide heater plate, Ceramic resistor, or similar heating element.
* **Power Supply:** External power source for the heater (do not power heaters directly from ESP32 pins!).

## 🔌 Wiring

**Warning:** The code uses `MOSFET_PIN 9`. On many ESP32 boards (like DOIT DevKit V1), **GPIO 9 is connected to the internal SPI Flash.** You must change this to a safe PWM-capable pin (e.g., GPIO 25, 26, 32, or 33).

### Voltage Divider (Thermistor)
The thermistor acts as a variable resistor. You must wire it in series with a fixed 10k resistor.

1.  **3.3V** -> One side of Thermistor
2.  **Signal Pin (A0/GPIO36)** -> Junction between Thermistor and 10k Resistor
3.  **GND** -> Other side of 10k Resistor

| Component | Pin Name | Code Definition | Recommended ESP32 Pin |
| :--- | :--- | :--- | :--- |
| **Thermistor** | Analog In | `THERMISTOR_PIN` | GPIO 36 (VP) |
| **MOSFET Gate** | PWM Out | `MOSFET_PIN` | GPIO 25 or 32 |



[Image of NTC thermistor voltage divider circuit]


### MOSFET Wiring
1.  **Gate:** To ESP32 Pin (via small resistor optional).
2.  **Drain:** To Heater Ground (Low side switching).
3.  **Source:** To Common Ground.



[Image of MOSFET low side switch wiring diagram]


## ⚙️ Configuration

### Tuning the PID
You can adjust the control behavior by modifying these variables at the top of the sketch:

```cpp
float kp = 1.58; // Proportional (Reaction speed)
float ki = 1.34; // Integral (Accumulated error correction)
float kd = 0.12; // Derivative (Damping/prediction)
