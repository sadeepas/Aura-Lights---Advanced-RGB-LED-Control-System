# Aura-Lights---Advanced-RGB-LED-Control-System
Aura Lights is a dual-component lighting control system designed for WS2812B (Neopixel) LED strips. It consists of a high-performance ESP32 Firmware backend that communicates via WebSockets and a modern, responsive Web Dashboard frontend built with Tailwind CSS.


Aura Lights - Advanced RGB LED Control System


Aura Lights is a dual-component lighting control system designed for WS2812B (Neopixel) LED strips. It consists of a high-performance ESP32 Firmware backend that communicates via WebSockets and a modern, responsive Web Dashboard frontend built with Tailwind CSS.

This system allows for real-time control of up to 6 distinct LED zones, featuring custom effects, color picking, and brightness management over a local WiFi network.

🌟 Features
🔌 ESP32 Firmware (Backend)

Multi-Zone Support: Controls up to 6 independent LED zones (default 60 LEDs per zone).

High-Speed Communication: Uses WebSockets (Port 81) for instant response times.

Smooth Effects: Integrated FastLED library for butter-smooth animations (Rainbow, Fade, Strobe, Chase).

WiFi Access Point: Creates its own WiFi network (AuraLights-RGB) for standalone operation.

Persistence: Saves zone names and configurations to non-volatile memory (Preferences).

💻 Web Dashboard (Frontend)

Modern UI: Glassmorphism design using Tailwind CSS.

Device Discovery: Support for WebSocket (WiFi) connections (UI also contains logic for Web Bluetooth).

Rich Controls:

RGB Color Wheel & Preset Colors.

Brightness & Effect Speed Sliders.

Effect Mode Selection.

Zone Naming & Categorization.

Event Logging: Real-time console log of commands and device responses.

Responsive: Works seamlessly on Desktop and Mobile.

🛠️ Hardware Requirements

Microcontroller: ESP32 Development Board (e.g., ESP32 DevKit V1).

LEDs: WS2812B Addressable RGB LED Strips.

Power Supply: 5V Power Supply (Amperage depends on the number of LEDs).

Connections: Data Pin connected to GPIO 16 (Configurable).

📦 Installation & Setup
1. Firmware Setup (ESP32)

Open the AuraLights.ino file in the Arduino IDE.

Install the following libraries via the Arduino Library Manager:

FastLED by Daniel Garcia.

WebSockets by Markus Sattler.

Configure the settings at the top of the file if necessary:

code
C++
download
content_copy
expand_less
#define NUM_ZONES       6
#define LEDS_PER_ZONE   60
#define DATA_PIN        16
const char* ssid = "AuraLights-RGB";
const char* password = "12345678";

Select your ESP32 board and upload the code.

Once uploaded, the ESP32 will broadcast a WiFi network named AuraLights-RGB.

2. Frontend Setup (Web Dashboard)

Save the LIGHTING CONTROL SYSTEM.html file to your computer or phone.

Connect your device to the AuraLights-RGB WiFi network (Password: 12345678).

Open LIGHTING CONTROL SYSTEM.html in any modern web browser (Chrome, Edge, Safari).

Click Connect -> Connect with WiFi.

Enter the Gateway IP (usually 192.168.4.1 for ESP32 AP mode) and connect.

📡 API Command Structure

The system uses a simple text-based protocol over WebSockets. You can use these commands to integrate with other systems like Home Assistant or Node-RED.

Command	Format	Description
Toggle	TOGGLE:<zone_id>	Turns a specific zone On/Off.
Brightness	BRIGHT:<zone_id>:<0-255>	Sets brightness for a zone.
Color	COLOR:<zone_id>:<HEX>	Sets color (e.g., COLOR:0:FF0000 for Red).
Effect	EFFECT:<zone_id>:<name>:<speed>	Sets effect (solid, fade, strobe, rainbow).
Name	NAME:<zone_id>:<new_name>	Renames a zone.
Get Config	GETCFG	Requests current configuration from device.
📂 File Structure
code
Code
download
content_copy
expand_less
.
├── AuraLights.ino                # ESP32 Firmware (Arduino C++)
├── LIGHTING CONTROL SYSTEM.html  # Web Interface (Single file HTML/JS/CSS)
└── README.md                     # Documentation
🤝 Contributing

Feel free to fork this project and submit pull requests. Areas for improvement include:

Adding OTA (Over-the-Air) update support to the firmware.

Implementing the Bluetooth Low Energy (BLE) backend in the C++ code to match the Frontend's capabilities.

Adding MQTT support for remote control.

👤 Author

Sadeepa Lakshan

GitHub

LinkedIn

📄 License

This project is open-source and available under the MIT License.
