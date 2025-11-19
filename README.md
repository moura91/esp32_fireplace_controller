# ESP32 Fireplace Remote Controller

A minimal IoT project for remotely controlling a fireplace using an **ESP32**, a **relay module**, and **MQTT**.  
The system also reads **temperature and humidity** using a **DHT11 sensor**, publishes the sensor values to MQTT, and allows remote operation of the relay via a mobile app.

---

## Features
- Reads **temperature & humidity** from DHT11
- Publishes sensor data to MQTT topic: `fireplace/sensor`
- Controls relay through MQTT topic: `fireplace/control`
- Remote control with android app

## Hardware Used
| Component | Description |
|-----------|-------------|
| ESP32-S2 | Main controller board |
| DHT11 sensor module | Temperature & humidity |
| Relay 5V module | Controls fireplace button |

### Wiring
| ESP32 Pin | Connected to |
|------------|--------------|
| GPIO 4 | DHT11 DATA |
| GPIO 5 | Relay IN |
| 3.3V | DHT11 VCC |
| 5V | Relay VCC |
| GND | Sensor + relay GND |

## Development Tools
| Tool | Description |
|--------|-------------|
| **ESP-IDF** | ESP32 development framework |
| **Visual Studio Code** | IDE used for coding & flashing |
| **ESP-IDF VSCode Extension** | Provides build, flash, monitor & menuconfig integration |
| **MQTT client tools** | For testing publish/subscribe in terminal |
| **Android MQTT App** | MQTT control dashboard on Android |

## Build & Flash
idf.py menuconfig --> Configure WiFi SSID, Password and MQTT Broker URI
idf.py set-target esp32s2
idf.py build
idf.py flash monitor

## Next Steps
- Secure broker with TLS & authentication


---

**Contact:** [LinkedIn] (https://www.linkedin.com/in/tiagosm)


