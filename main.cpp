/**
 * @file main.cpp
 * @brief Production-Grade IR Cloner Firmware for ESP8266 (D1 Mini)
 * @details
 *      Board: Wemos D1 Mini
 *      IR Rx: VS1838B on Pin D5 (GPIO 14)
 *      IR Tx: IR LED on Pin D2 (GPIO 4)
 *      Btn 1: Capture (Pull-up) on Pin D6 (GPIO 12)
 *      Btn 2: Send (Pull-up) on Pin D7 (GPIO 13)
 *      LED: Built-in LED on Pin D4 (GPIO 2) - Active Low
 */

#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRsend.h>
#include <IRutils.h>
#include <vector>

// ===================================================================================
// CONFIGURATION
// ===================================================================================

// Pins
const uint16_t kRecvPin = 14;      // D5 (GPIO 14)
const uint16_t kSendPin = 4;       // D2 (GPIO 4)
const uint16_t kBtnCapturePin = 12;// D6 (GPIO 12)
const uint16_t kBtnSendPin = 13;   // D7 (GPIO 13)
const uint16_t kLedPin = 2;        // D4 (GPIO 2) - Built-in LED

// IR Settings
const uint16_t kCaptureBufferSize = 1200; // Expanded for AC remotes
const uint8_t  kTimeout = 50;             // 50ms timeout
const uint16_t kMinUnknownSize = 12;      // Min valid signal
const uint32_t kFrequency = 38;           // 38kHz carrier

// Globals
IRrecv irrecv(kRecvPin, kCaptureBufferSize, kTimeout, true);
IRsend irsend(kSendPin);
decode_results results;
std::vector<uint16_t> capturedSignal;

// ===================================================================================
// UTILS: Button Class (Non-Blocking)
// ===================================================================================
class Button {
  private:
    uint8_t pin;
    bool state;
    bool lastState;
    unsigned long lastDebounceTime;
    unsigned long debounceDelay;

  public:
    Button(uint8_t p, unsigned long debounce = 50) : pin(p), debounceDelay(debounce) {
        pinMode(pin, INPUT_PULLUP);
        state = HIGH;
        lastState = HIGH;
        lastDebounceTime = 0;
    }

    // Returns true ONLY on falling edge (Press)
    bool isPressed() {
        bool reading = digitalRead(pin);
        bool pressed = false;

        if (reading != lastState) {
            lastDebounceTime = millis();
        }

        if ((millis() - lastDebounceTime) > debounceDelay) {
            if (reading != state) {
                state = reading;
                if (state == LOW) {
                    pressed = true;
                }
            }
        }
        lastState = reading;
        return pressed;
    }
};

// ===================================================================================
// UTILS: LED Controller (Non-Blocking)
// ===================================================================================
class LedController {
  private:
    uint8_t pin;
    unsigned long lastUpdate;
    bool state;
    bool active; // Is blinking active?
    unsigned long interval;

  public:
    LedController(uint8_t p) : pin(p) {
        pinMode(pin, OUTPUT);
        off(); // Default OFF
    }

    void on() {
        active = false;
        digitalWrite(pin, LOW); // Active Low
    }

    void off() {
        active = false;
        digitalWrite(pin, HIGH); // Active Low
    }

    void startBlink(unsigned long ms) {
        if (!active || interval != ms) {
            active = true;
            interval = ms;
            lastUpdate = 0; // Force immediate update
            state = HIGH;   // Start OFF
        }
    }

    void update() {
        if (!active) return;
        
        if (millis() - lastUpdate >= interval) {
            lastUpdate = millis();
            state = !state;
            digitalWrite(pin, state ? HIGH : LOW);
        }
    }
};

// Instantiate Objects
Button btnCapture(kBtnCapturePin);
Button btnSend(kBtnSendPin);
LedController statusLed(kLedPin);

// ===================================================================================
// MAIN LOGIC
// ===================================================================================

enum SystemState {
    STATE_IDLE,
    STATE_CAPTURING,
    STATE_SENDING
};
SystemState currentState = STATE_IDLE;

void setup() {
    Serial.begin(115200);
    // Wait for Serial to stabilize
    delay(500);
    Serial.println();
    Serial.println("[SYS] ================================");
    Serial.println("[SYS]    IR CLONER FIRMWARE V2.0      ");
    Serial.println("[SYS] ================================");

    irrecv.setUnknownThreshold(kMinUnknownSize);
    irsend.begin();

    Serial.printf("[SYS] IR Recv: D%d, IR Send: D%d\n", kRecvPin, kSendPin);
    Serial.println("[SYS] Ready. Press Capture (Btn1) or Send (Btn2).\n");
}

void loop() {
    // 1. Update Hardware Helpers
    statusLed.update();

    // 2. State Machine
    switch (currentState) {
        // -------------------------------------------------------------------
        // IDLE
        // -------------------------------------------------------------------
        case STATE_IDLE:
            // Check Inputs
            if (btnCapture.isPressed()) {
                Serial.println("[USR] Capture Button Pressed -> Mode: CAPTURING");
                capturedSignal.clear();
                irrecv.enableIRIn(); // Start sampling
                statusLed.startBlink(100); // Fast blink
                currentState = STATE_CAPTURING;
            } else if (btnSend.isPressed()) {
                if (capturedSignal.empty()) {
                    Serial.println("[ERR] Buffer empty! Capture a signal first.");
                    // Flash LED briefly to indicate error
                    statusLed.on(); delay(100); statusLed.off(); delay(100);
                    statusLed.on(); delay(100); statusLed.off();
                } else {
                    Serial.println("[USR] Send Button Pressed -> Mode: SENDING");
                    currentState = STATE_SENDING;
                }
            }
            break;

        // -------------------------------------------------------------------
        // CAPTURING
        // -------------------------------------------------------------------
        case STATE_CAPTURING:
            if (irrecv.decode(&results)) {
                Serial.println("[IR]  Signal Detected!");
                
                // Check for overflow
                if (results.overflow) {
                     Serial.println("[WARN] Buffer overflow! Signal may be truncated. Increase kCaptureBufferSize.");
                }

                // Convert raw ticks to microseconds
                // We skip index 0 (gap) and start from 1
                uint16_t count = results.rawlen;
                if (count > 0) {
                    capturedSignal.reserve(count - 1);
                    for (uint16_t i = 1; i < count; i++) {
                        uint32_t usecs = results.rawbuf[i] * kRawTick;
                        // Clamp to uint16_t max to prevent overflow issues in storage
                        if (usecs > 65535) usecs = 65535; 
                        capturedSignal.push_back((uint16_t)usecs);
                    }
                }

                Serial.printf("[MEM] Stored %d samples.\n", capturedSignal.size());
                
                // Debug Output
                Serial.print("[DBG] Raw: ");
                for (size_t i = 0; i < capturedSignal.size(); i++) {
                    Serial.print(capturedSignal[i]);
                    if (i < capturedSignal.size() - 1) Serial.print(",");
                }
                Serial.println();

                // Cleanup
                irrecv.disableIRIn(); // Stop receiver to avoid noise
                statusLed.off();
                currentState = STATE_IDLE;
                Serial.println("[SYS] Returned to IDLE.");
            }
            
            // Allow Exit
            if (btnCapture.isPressed() || btnSend.isPressed()) {
                Serial.println("[USR] Capture Cancelled.");
                irrecv.disableIRIn();
                statusLed.off();
                currentState = STATE_IDLE;
            }
            break;

        // -------------------------------------------------------------------
        // SENDING
        // -------------------------------------------------------------------
        case STATE_SENDING:
            statusLed.on(); // Solid ON
            Serial.printf("[TX]  Sending %d samples @ %dkHz...\n", capturedSignal.size(), kFrequency);
            
            // Blocking call, but acceptable for IR TX (~100-500ms)
            irsend.sendRaw(capturedSignal.data(), capturedSignal.size(), kFrequency);
            
            Serial.println("[TX]  Complete.");
            statusLed.off();
            currentState = STATE_IDLE;
            break;
    }
}
