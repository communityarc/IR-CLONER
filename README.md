```txt
██╗██████╗      ██████╗██╗      ██████╗ ███╗   ██╗███████╗██████╗
██║██╔══██╗    ██╔════╝██║     ██╔═══██╗████╗  ██║██╔════╝██╔══██╗
██║██████╔╝    ██║     ██║     ██║   ██║██╔██╗ ██║█████╗  ██████╔╝
██║██╔══██╗    ██║     ██║     ██║   ██║██║╚██╗██║██╔══╝  ██╔══██╗
██║██║  ██║    ╚██████╗███████╗╚██████╔╝██║ ╚████║███████╗██║  ██║
╚═╝╚═╝  ╚═╝     ╚═════╝╚══════╝ ╚═════╝ ╚═╝  ╚═══╝╚══════╝╚═╝  ╚═╝
```

## IR Cloner Firmware (ESP8266 – D1 Mini)

Production-grade infrared (IR) cloner firmware for the ESP8266 (Wemos D1 Mini) that captures raw IR signals from original remote controls and accurately retransmits them on demand using hardware buttons and a non-blocking state machine.

## Features
- Raw IR signal capture (protocol-agnostic)
- Accurate IR retransmission using sendRaw()
- Supports long and complex signals (AC remotes)
- Non-blocking button handling with debounce
- Non-blocking LED status indication
- Clean state-machine-based architecture

## Hardware
- Wemos D1 Mini (ESP8266)
- VS1838B / TSOP IR Receiver
- IR LED + resistor
- 2 Push Buttons
- Built-in LED


