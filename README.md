# 🔥 Wearable Fire Escape Guidance Device (Arduino + MYOSA Mini Kit)

This project is a **wearable fire escape guidance system** built using the **MYOSA Mini Kit**.  
It helps people navigate back to safety in low-visibility emergencies (like fire or smoke) using **sensors + guidance instructions**.

---

## 🚀 Features
- Uses **MPU6050 (Gyro/Accel)** to detect steps and direction.
- Uses **BMP180** for **pressure and temperature monitoring**.
- Uses **APDS9960** for **light level detection** (low, medium, high).
- OLED screen displays status and guidance:
  - "NO FIRE" and sensor readings in cycle (3s gap).
  - "FIRE!!" when hazard is triggered.
  - Escape route instructions step-by-step until EXIT.
- **Buzzer alarm** starts when fire is detected and stops only after safe exit.
- Supports **setup mode** to mark Entrance (E), Junction (J), and Room (R).
- Supports **reset (N)** for a new setup.

---

## 📟 Demo Flow
1. **Initialization**
   - Set entrance with `E`
   - Walk 3 steps, set junction with `J`
   - Walk 5 steps, set room with `R`
   - OLED shows **Setup Done**

2. **Normal Mode**
   - OLED cycles:
     - `NO FIRE`
     - `P: Pressure`
     - `T: Temperature`
     - `L: Light Level (LOW/MEDIUM/HIGH)`

3. **Hazard**
   - Trigger fire using `H` in Serial (or via sensor threshold in real-life).
   - OLED shows **"FIRE!!"**
   - Buzzer ON

4. **Escape Guidance**
   - Device guides user step-by-step back to Exit:
     - "Turn Around"
     - "Walk 5 steps"
     - "Turn Left"
     - "Walk 3 steps"
   - At Exit → OLED shows **EXIT** for 3s
   - Buzzer stops, system resets to **NO FIRE**

---

## 🛠 Hardware
- **MYOSA Mini Kit** (ESP32-based)
- Sensors:
  - MPU6050 (step + direction)
  - BMP180 (pressure + temperature)
  - APDS9960 (light level)
- OLED Display (SSD1306, I2C)
- Buzzer

---

## 📂 Code
The Arduino sketch (`MYOSA.ino`) contains:
- Setup of sensors and OLED
- Step + turn simulation
- Hazard detection and guidance sequence

---

## ⚙️ Setup
1. Install [Arduino IDE](https://www.arduino.cc/en/software).
2. Install required libraries (see `requirements.txt`).
3. Connect your **MYOSA Mini Kit** via USB.
4. Upload `fire_escape_guidance.ino` to the ESP32.
5. Open **Serial Monitor** for setup commands:
   - `E` → Set Entrance
   - `J` → Set Junction
   - `R` → Set Room
   - `H` → Trigger Hazard
   - `N` → New Setup / Reset

---

## 📜 License
This project is licensed under the [MIT License](LICENSE).
