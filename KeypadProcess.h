#ifndef __KEYPAD_PROCESS_H__
#define __KEYPAD_PROCESS_H__

#include <Keypad.h>
#include <ProcessScheduler.h>

const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};

Keypad keypad = Keypad(makeKeymap(keys), KEY_ROW, KEY_COL, ROWS, COLS);

class KeypadProcess : public Process
{
  public:
    KeypadProcess(Scheduler &manager, ProcPriority pr, unsigned int period)
        : Process(manager, pr, period)
    {
        _lastKey = NO_KEY;
    }

    void setup(void);
    void service(void);

  private:
    char _lastKey;
};

void KeypadProcess::setup()
{
    keypad.addEventListener(keypadEvent);
}

void service()
{
    _lastkey = keypad.getKey();
}

void keypadEvent(KeypadEvent key)
{
    switch (keypad.getState())
    {
    case PRESSED:
        if (key == '#')
        {
            digitalWrite(ledPin, !digitalRead(ledPin));
            ledPin_state = digitalRead(ledPin); // Remember LED state, lit or unlit.
        }
        break;

    case RELEASED:
        if (key == '*')
        {
            digitalWrite(ledPin, ledPin_state); // Restore LED state from before it started blinking.
            blink = false;
        }
        break;

    case HOLD:
        if (key == '*')
        {
            blink = true; // Blink the LED when holding the * key.
        }
        break;
    }
}

#endif
