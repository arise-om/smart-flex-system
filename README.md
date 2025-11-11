# Smart Flex System - Sign Language to Text

**IEEM Mini Project | AISSMS College of Engineering, Pune**  
**SE R&A Batch-C | Guide: R. Wani Sir**

---

## 📋 Overview

The Smart Flex System is an innovative Arduino-based project that converts finger bend gestures (detected via a flex sensor) into text messages. The system displays real-time status on an LCD screen and communicates via Bluetooth, making it ideal for assistive communication devices and sign language interpretation.

## ✨ Features

- **Flex Sensor-Based Gesture Recognition** - Detects finger bending and converts to status messages
- **LCD Display** - Real-time status display on 16x2 I2C LCD ("I am Fine" / "Need Help!")
- **Bluetooth Communication** - Wireless data transmission and remote command interface
- **Buzzer Alerts** - Audio feedback for emergency detection
- **EEPROM Calibration Storage** - Persistent sensor calibration across power cycles
- **Command Interface** - Full-featured serial terminal with multiple commands
- **Auto-Calibration** - Guided calibration process with live ADC value display

## 🔧 Hardware Required

| Component | Specification | Connection |
|-----------|---------------|------------|
| **Microcontroller** | Arduino Uno | - |
| **Flex Sensor** | 2.2" / 4.5" | Analog Pin A0 |
| **LCD Display** | 16x2 I2C | SDA, SCL |
| **Bluetooth Module** | HC-05 | RX: Pin 10, TX: Pin 11 |
| **Buzzer** | Active/Passive | Digital Pin 9 |
| **Power Supply** | 9V Battery / USB | - |

## 📁 Repository Structure

```
smart-flex-system/
├── smart_flex_system.ino    # Main Arduino firmware (v8.1)
├── README.md                # Project documentation
└── .gitignore              # Git ignore file
```

## 🚀 Getting Started

### Prerequisites

- Arduino IDE (v1.8.x or higher)
- Required Libraries:
  - `Wire.h` (built-in)
  - `LiquidCrystal_I2C.h` ([Download](https://github.com/johnrickman/LiquidCrystal_I2C))
  - `SoftwareSerial.h` (built-in)
  - `EEPROM.h` (built-in)

### Installation Steps

1. **Clone the repository**
   ```bash
   git clone https://github.com/arise-om/smart-flex-system.git
   cd smart-flex-system
   ```

2. **Install required libraries**
   - Open Arduino IDE
   - Go to `Sketch → Include Library → Manage Libraries`
   - Search and install `LiquidCrystal_I2C`

3. **Wire the circuit** (see circuit diagram below)

4. **Upload the code**
   - Open `smart_flex_system.ino` in Arduino IDE
   - Select `Tools → Board → Arduino Uno`
   - Select the correct COM port
   - Click **Upload**

5. **First Run Calibration**
   - Connect via Bluetooth or Serial Monitor (9600 baud)
   - System will auto-start calibration if needed
   - Follow on-screen instructions

## 📟 Bluetooth Commands

Connect via Bluetooth terminal (9600 baud) and use these commands:

| Command | Description |
|---------|-------------|
| `HELP` | Display all available commands |
| `CAL` | Start manual calibration process |
| `STATUS` | Show current sensor calibration values |
| `RESET` | Reset calibration to defaults |
| `INFO` | Display system and build information |
| `ECHOON` | Enable serial output (default: ON) |
| `ECHOOFF` | Disable serial output |
| `BUZZON` | Enable buzzer alerts (default: ON) |
| `BUZZOFF` | Disable buzzer alerts |

## 🎯 Usage

1. **Power On** - System displays welcome screens
2. **Calibration** - Auto-calibrates on first use
3. **Normal Operation** - Displays "I am Fine." when flex sensor is straight
4. **Help Signal** - Displays "Need Help!" and triggers buzzer when sensor is bent beyond threshold (75%)
5. **Bluetooth Monitoring** - Real-time data streaming to connected device

## 🔬 Calibration Process

1. Send `CAL` command via Bluetooth
2. **Step 1:** Keep sensor STRAIGHT for 5 seconds (watch live ADC values)
3. **Step 2:** BEND sensor fully for 5 seconds
4. System automatically saves calibration to EEPROM
5. Confirmation beep and LCD message

## 🛠️ Troubleshooting

| Issue | Solution |
|-------|----------|
| LCD not displaying | Check I2C address (default: 0x27), verify SDA/SCL connections |
| Bluetooth not connecting | Ensure HC-05 is paired, check baud rate (9600) |
| Inaccurate readings | Run `CAL` command for recalibration |
| No buzzer sound | Check buzzer polarity, verify pin 9 connection, try `BUZZON` |
| Values not saving | EEPROM issue - power cycle and recalibrate |

## 📊 Technical Specifications

- **Sensor Range:** 0-1023 ADC units
- **Bend Detection Threshold:** 75% (configurable in code)
- **Update Rate:** ~300ms (configurable)
- **Bluetooth Baud Rate:** 9600
- **Serial Baud Rate:** 9600
- **Power Consumption:** ~150mA @ 5V
- **Operating Voltage:** 5V DC

## 🎓 Project Information

**Institution:** AISSMS College of Engineering, Pune  
**Department:** Electronics Engineering  
**Batch:** SE R&A, Batch-C  
**Project Type:** IEEM (Innovation in Electronics Engineering & Management) Mini Project  
**Guide:** Prof. R. Wani Sir  
**Version:** 8.1  
**Date:** November 2025

## 📝 Code Structure

The firmware is organized into functional sections:

- **Pin Configuration** - Hardware pin definitions
- **Object Initialization** - LCD, Bluetooth, and sensor setup
- **Calibration System** - EEPROM storage and auto-calibration
- **Welcome Screens** - Startup display sequence
- **Main Loop** - Banner scrolling, sensor reading, status display
- **Command Handler** - Bluetooth command processing
- **Utility Functions** - Buzzer patterns, display helpers

## 🤝 Contributing

Contributions, issues, and feature requests are welcome! Feel free to check the [issues page](https://github.com/arise-om/smart-flex-system/issues).

## 📄 License

This project is open-source and available for educational purposes.

## 👥 Team

Developed by SE R&A students at AISSMS COE, Pune.

## 📞 Contact

For queries or collaboration:
- **GitHub:** [@arise-om](https://github.com/arise-om)
- **Repository:** [smart-flex-system](https://github.com/arise-om/smart-flex-system)

---

**⭐ If you find this project helpful, please consider giving it a star!**
