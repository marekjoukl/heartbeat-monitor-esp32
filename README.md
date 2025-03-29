# ESP32 Heartbeat Monitor ❤️‍🩹

This project is a compact heart rate monitor built using an **ESP32 (Wemos D1 R32)** microcontroller, a **PulseSensor** heart rate sensor, and an **OLED SSD1306** display.

🛠️ Created as part of the **Microprocessor and Embedded Systems (IMP)** course at Brno University of Technology (2024/2025).

📹 Demo Video: https://drive.google.com/file/d/1XfVxuwtyYrqaY6fT22sxy4qXZxiEHp-n/view?usp=sharing

---

## 🚀 Features

- Real-time heart rate monitoring via analog input
- BPM (Beats Per Minute) and average BPM display on OLED
- Live waveform plotting of heart rate signal
- Noise filtering and signal thresholding
- Automatic debounce and peak detection

---

## ⚙️ Hardware Used

- **ESP32 (Wemos D1 R32)**
- **Pulse Sensor** (analog output)
- **OLED SSD1306 Display (I2C)**

### 🔌 Wiring

**Pulse Sensor**

- VCC → 5V
- GND → GND
- Signal → GPIO36 (A0)

**OLED Display**

- VCC → 5V
- GND → GND
- SDA → SDA
- SCL → SCL

---

## 📦 Getting Started

1. Open the project in **Arduino IDE**
2. Install required libraries:
   - `Adafruit_GFX`
   - `Adafruit_SSD1306`
   - `Wire`
3. Select board: `Wemos D1 R32`
4. Set correct port and upload

---

## 📊 Signal Processing

- The analog pulse signal is filtered with a low-pass filter
- Beats are detected when signal crosses a threshold upward and drops below again
- BPM is computed using beat intervals
- A moving average is used for displaying average BPM
- Live waveform is plotted on the OLED screen in real-time

---

## ✅ Testing & Validation

The device was tested against commercial smartwatches for accuracy under:

- Resting conditions
- Post-exercise states

Example Results:

| Person | My Device (Rest) | Watch (Rest) | My Device (After) | Watch (After) |
| ------ | ---------------- | ------------ | ----------------- | ------------- |
| 1      | 62 BPM           | 65 BPM       | 112 BPM           | 117 BPM       |
| 2      | 71 BPM           | 73 BPM       | 132 BPM           | 136 BPM       |

---

## 👨‍💻 Author

**Marek Joukl**

---

## 📜 License

MIT License – Feel free to modify or expand!

---

## 🔗 References

- https://lastminuteengineers.com/pulse-sensor-arduino-tutorial/
- https://github.com/adafruit/Adafruit-GFX-Library
- https://github.com/adafruit/Adafruit_SSD1306
