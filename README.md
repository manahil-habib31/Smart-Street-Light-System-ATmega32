# 🌙 Smart Street Light System using ATmega32

An intelligent street lighting system developed using **Embedded C** and the **ATmega32 microcontroller**. The system automatically controls street lights based on ambient light conditions and motion detection, reducing energy consumption while maintaining road safety.

---

## 📌 Project Overview

The Smart Street Light System is an embedded systems project that automates street lighting using an LDR sensor and motion detection logic.

The system operates in three different modes:

- **Day Mode:** Street light remains OFF.
- **Night Mode:** Street light operates in DIM mode.
- **Motion Detection Mode:** When motion is detected at night, the street light switches to FULL brightness and a buzzer sounds briefly. After 30 seconds of no motion, the light returns to DIM mode.

A **16×2 LCD** continuously displays the current operating status.

This project was developed using **Embedded C** in **Microchip Studio 7** and simulated in **SimulIDE 1.1.0**.

---

# ✨ Features

- 🌞 Automatic Day/Night Detection
- 🚶 Motion Detection using External Interrupt (INT0)
- 💡 Automatic Brightness Control
- 🔆 DIM Mode during Night
- ⚡ FULL Brightness on Motion Detection
- 🔔 Buzzer Alert
- 📟 16×2 LCD Status Display
- ⏲️ Automatic 30-Second Motion Timeout
- ⚙️ Interrupt-Based Embedded Programming

---

# 🛠 Hardware Components

- ATmega32 Microcontroller
- LDR (Light Dependent Resistor)
- Push Button (PIR Sensor Simulation)
- LED
- Buzzer
- 16×2 LCD Display
- 10kΩ Resistors
- 330Ω Resistor
- 5V Power Supply

---

# 💻 Software & Tools

- Embedded C
- Microchip Studio 7
- SimulIDE 1.1.0
- AVR Libraries

---

# 📂 Project Structure

```
Smart-Street-Light-System-ATmega32/
│
├── Code/
│   └── street_light.c
│
├── Circuit/
│   └── circuit_diagram.png
│
├── Simulation/
│   ├── day_mode.png
│   ├── night_mode.png
│   └── motion_detected.png
│
├── README.md
├── LICENSE
└── .gitignore
```

---

# ⚙️ System Working

## 🌞 Day Mode

- LDR detects daylight.
- Street light remains OFF.
- Motion detection is ignored.

**LCD Display**

```
MODE: DAY
LED: OFF
```

---

## 🌙 Night Mode

- LDR detects darkness.
- Street light switches to DIM mode.
- System waits for motion.

**LCD Display**

```
MODE: NIGHT
LED: DIM
```

---

## 🚶 Motion Detection Mode

When motion is detected during nighttime:

- LED changes to FULL brightness.
- Buzzer beeps once.
- LCD displays the motion status.

**LCD Display**

```
MODE: NIGHT
MOTION DETECT!
```

After 30 seconds without further motion, the system automatically returns to DIM mode.

---

# 🔄 System Flow

```
                    Start
                      │
                      ▼
               Read LDR Sensor
                      │
          ┌───────────┴───────────┐
          │                       │
        Day                    Night
          │                       │
      LED OFF             Check Motion
                                  │
                    ┌─────────────┴─────────────┐
                    │                           │
               No Motion                   Motion
                    │                           │
               LED DIM                  LED FULL
             LCD Update               LCD Update
             Buzzer OFF               Buzzer Beep
```

---

# 🧠 Embedded Concepts Used

- Embedded C Programming
- GPIO Configuration
- ADC (Analog-to-Digital Converter)
- External Interrupt (INT0)
- Timer0 Interrupt
- Interrupt Service Routines (ISR)
- LCD Interfacing (4-Bit Mode)
- Sensor Interfacing
- State Machine Design
- Power-Efficient Embedded Systems

---

# 📸 Project Images

## Circuit Diagram

```
Circuit/Initialization.png
```

## Simulation Results

- Day Mode

```
Simulation/DayTime.png
```

- Night Mode

```
Simulation/Night_No_Motion.png
```

- Motion Detection Mode

```
Simulation/Night_MotionDetected.png
```

---

# 🚀 Future Improvements

- Replace the push button with a real PIR motion sensor.
- Implement PWM for smoother LED brightness control.
- Integrate solar-powered energy management.
- Add IoT-based remote monitoring and control.
- Support multiple synchronized street lights.
- Implement automatic fault detection and maintenance alerts.

---

# 👩‍💻 Author

**Manahil Habib**

Software Engineering Student

### Areas of Interest

- Embedded Systems
- Artificial Intelligence
- Cybersecurity
- Automation
- Internet of Things (IoT)

---

## ⭐ Support

If you found this project useful or interesting, consider giving it a **⭐ Star** on GitHub!