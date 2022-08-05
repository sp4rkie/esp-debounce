
//
// tested on 
//     LOLIN(WEMOS) D1 R2 & mini
// and
//     WEMOS D1 MINI ESP32
//

#include "mylcf.h"
#include <Ticker.h>

#ifdef ESP8266
#define GPIO_KEY 4              // define key to debounce
#else
#define GPIO_KEY 27
#endif

#define DEBOUNCE_MASK   0xff00  // debounces key press, implies 8 equal samples in a row for a key to trigger
#define DEBOUNCE_MASKr  0xfeff  // debounces key release, implies 8 equal samples in a row for a key to trigger
#define DEBOUNCE_RATE        3  // sample rate in ms to scan the key state 
#define DEBOUNCE_CNT       100  // max. cnt of debounces => debounce timeout == DEBOUNCE_RATE x (DEBOUNCE_CNT - 1)

#define KEY_NONE      0         // no change
#define KEY_RELEASED  1         // key has been released
#define KEY_PRESSED   2         // key has been depressed

class Button
{
    private:
    public:
        _u8 button;
        _u16 state = ~0;        // start with GPIO samples all open ( == 1) 
        _u8 debounces;
        void 
        begin(_u8 btn) {
            reset();
            button = btn;
            pinMode(button, INPUT_PULLUP);
        }
        void 
        reset() {
//            state = ~0;       // don't reset during operation
            debounces = 0;  
        }
        _u8 
        debounce() {
            ++debounces;        // debounces run so far
            state = state << 1 | digitalRead(button) | DEBOUNCE_MASK << 1;
            return state == DEBOUNCE_MASK ? KEY_PRESSED : state == DEBOUNCE_MASKr ? KEY_RELEASED : KEY_NONE;
        }
};

Button button;
Ticker timer;
_u8 key_state;

void
samplekey()
{
    _u8 k_stat = button.debounce();

Serial.printf(".%x", button.state);
    if (k_stat != KEY_NONE) {
        key_state = k_stat;
Serial.printf(">%x", key_state);
    }
    if (k_stat != KEY_NONE || button.debounces > DEBOUNCE_CNT) {
        // reset if either key is pressed/released or debounce timed out
        timer.detach();
        button.reset();
    }
}

void IRAM_ATTR 
keyintr() 
{
    if (!button.debounces) {
        timer.attach_ms(DEBOUNCE_RATE, samplekey);
        button.debounce();          // take the initial sample right here
    }
}

void 
setup() 
{
    Serial.begin(115200);
    button.begin(GPIO_KEY);
    attachInterrupt(digitalPinToInterrupt(GPIO_KEY), keyintr, CHANGE);  // must handle both directions for release and press

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
}

void 
loop() 
{
    static _u8 old_keystate;

    if (key_state != old_keystate) {
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
        Serial.printf("new key_state: %d\n", key_state);
        old_keystate = key_state;
    }
}

