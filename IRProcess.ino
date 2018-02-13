#include <Arduino.h>

#include "IRProcess.h"
#include "Keys.h"

void IRProcess::setup()
{
    _irrecv->enableIRIn();
}

void IRProcess::service()
{
#ifdef _DEBUG_
    unsigned long b_t = millis();
#endif

    char value = getch();
    if (value != KEY_NONE && _func_callback != NULL)
        _func_callback(value);

#ifdef _DEBUG_
    Serial.println("IR Process: " + String(millis() - b_t));
#endif
}

char IRProcess::getch()
{
    char ret = KEY_NONE;
    decode_results results;

    if (_irrecv->decode(&results))
    {
        if (!results.overflow)
            ret = convertNEC(results.value);
        _irrecv->resume();
    }
    return ret;
}

char IRProcess::convertNEC(unsigned long code)
{
    switch (code)
    {
    case 0xFFA25D: // Вкл
        return KEY_ON;
    case 0xFF629D: // Mode
        return KEY_MODE;
    case 0xFFE21D:        // Звук выкл
        return KEY_SOUND; // Escape
    case 0xFF22DD:        // Play/pause
        return KEY_PLAY;
    case 0xFF02FD: // Предыдущий
        return KEY_PREV;
    case 0xFFC23D: // Следующий
        return KEY_NEXT;
    case 0xFFE01F: // EQ
        return KEY_EQ;
    case 0xFFA857: // '-'
        return KEY_MINUS;
    case 0xFF906F: // '+'
        return KEY_PLUS;
    case 0xFF9867:        // Стрелки туда-сюда
        return KEY_ARROW; // Backspace
    case 0xFFB04F:        // U/SD
        return KEY_U_SD;  // Enter
    case 0xFF6897:        // '0'
        return KEY_0;
    case 0xFF30CF: // '1'
        return KEY_1;
    case 0xFF18E7: // '2'
        return KEY_2;
    case 0xFF7A85: // '3'
        return KEY_3;
    case 0xFF10EF: // '4'
        return KEY_4;
    case 0xFF38C7: // '5'
        return KEY_5;
    case 0xFF5AA5: // '6'
        return KEY_6;
    case 0xFF42BD: // '7'
        return KEY_7;
    case 0xFF4AB5: // '8'
        return KEY_8;
    case 0xFF52AD: // '9'
        return KEY_9;
    default:
        break;
    }
    return KEY_NONE;
}
