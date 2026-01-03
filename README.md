██╗██████╗      ██████╗██╗      ██████╗ ███╗   ██╗███████╗██████╗
██║██╔══██╗    ██╔════╝██║     ██╔═══██╗████╗  ██║██╔════╝██╔══██╗
██║██████╔╝    ██║     ██║     ██║   ██║██╔██╗ ██║█████╗  ██████╔╝
██║██╔══██╗    ██║     ██║     ██║   ██║██║╚██╗██║██╔══╝  ██╔══██╗
██║██║  ██║    ╚██████╗███████╗╚██████╔╝██║ ╚████║███████╗██║  ██║
╚═╝╚═╝  ╚═╝     ╚═════╝╚══════╝ ╚═════╝ ╚═╝  ╚═══╝╚══════╝╚═╝  ╚═╝







IR Cloner Firmware (ESP8266 – D1 Mini)

Production-grade infrared (IR) cloner firmware for the ESP8266 (Wemos D1 Mini) that captures raw IR signals from original remotes and accurately retransmits them on demand using hardware buttons and a non-blocking state machine.

📌 Features

✅ Raw IR signal capture (protocol-agnostic)

✅ Accurate retransmission using sendRaw()

✅ Supports complex remotes (AC / long frames)

✅ Non-blocking button handling (debounced)

✅ Non-blocking LED status indicator

✅ Clear state machine architecture

✅ Designed for real-world use, not a demo

🧠 How It Works

Capture Mode

Listens for an incoming IR signal

Stores raw pulse timings in microseconds

Supports long signals (AC remotes)

Send Mode

Replays the captured raw IR signal

Uses a 38 kHz carrier frequency

Maintains timing accuracy

State Machine

IDLE

CAPTURING

SENDING

This ensures predictable behavior and prevents blocking or unstable states.

🔧 Hardware Requirements
Component	Description
Microcontroller	Wemos D1 Mini (ESP8266)
IR Receiver	VS1838B / TSOP series
IR Transmitter	IR LED + resistor
Buttons	2 × Push buttons (Pull-up)
LED	Built-in LED (D4, Active-Low)
🔌 Pin Configuration
Function	ESP8266 Pin	GPIO
IR Receiver	D5	GPIO 14
IR Transmitter	D2	GPIO 4
Capture Button	D6	GPIO 12
Send Button	D7	GPIO 13
Status LED	D4	GPIO 2

Buttons use internal pull-ups (active LOW)

⚙️ Software Requirements

Arduino IDE

ESP8266 Board Package

IRremoteESP8266 library

Required Libraries
IRremoteESP8266
Arduino STL (vector)

🛠️ Installation

Install ESP8266 board support in Arduino IDE

Install IRremoteESP8266 library via Library Manager

Clone this repository:

git clone https://github.com/yourusername/ir-cloner-esp8266.git


Open main.cpp in Arduino IDE

Select Wemos D1 Mini as the board

Upload the firmware

▶️ Usage
Capture an IR Signal

Point original remote at the IR receiver

Press Capture Button

Status LED blinks fast

Signal is stored automatically

Send the Captured Signal

Press Send Button

IR signal is transmitted

Status LED turns solid during transmission

💡 LED Status Indicators
LED State	Meaning
OFF	Idle
Fast Blink	Capturing IR
Solid ON	Transmitting IR
Short Blinks	Error (no signal stored)
🧪 Debug & Logging

Serial baud rate: 115200

Raw IR timings are printed for verification

Overflow warnings shown if buffer limit is reached

⚠️ Notes & Limitations

Stores one IR signal at a time

Uses RAM (vector) — power cycle clears memory

Extremely long AC signals may require buffer tuning

Blocking send (~100–500 ms) is intentional for timing accuracy

🔐 Intended Use

Remote control cloning

Device automation

IR protocol research

Embedded systems learning

Ethical security & hardware testing

⚠️ Use responsibly and only on devices you own or are authorized to test.

📄 License

MIT License
Feel free to modify, use, and distribute with attribution.

👤 Author

IR Cloner Firmware – ESP8266
Designed for stability, accuracy, and real-world deployment.


BY COMMUNITY ARC
