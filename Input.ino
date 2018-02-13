#include <Arduino.h>
#include <stdlib.h>

#include "Input.h"

void Input::draw()
{
    cursor_x = left;
    cursor_y = top;
    if (strlen(label) > 0)
    {
        myLCD->print(label, cursor_x, cursor_y);
        cursor_x += strlen(label) * _fontXSize + 3;
    }
    if (strlen(value) > 0)
    {
        myLCD->print(value, cursor_x, cursor_y);
        myLCD->drawRect(cursor_x - 1, cursor_y - 1, cursor_x + strlen(value) * _fontXSize + 1, cursor_y + _fontYSize + 1);
        showCursor();
    }
}

boolean Input::add_char(char ch)
{
    value[cursor] = ch;
    myLCD->printChar(ch, cursor_x, cursor_y);
    hideCursor();
    cursor++;
    cursor_x += _fontXSize;
    while (value[cursor] == delimiter)
    {
        cursor++;
        cursor_x += _fontXSize;
    }

    if (cursor >= strlen(value))
        return true;
    showCursor();
    return false;
}

void Input::remove_char()
{
    if (cursor == 0)
        return;
    hideCursor();
    cursor--;
    cursor_x -= _fontXSize;
    while (value[cursor] == delimiter && cursor > 0)
    {
        cursor--;
        cursor_x -= _fontXSize;
    }
    myLCD->printChar(filler, cursor_x, cursor_y);
    showCursor();
}

void Input::showCursor()
{
    myLCD->fillRect(cursor_x, cursor_y + _fontYSize - 1, cursor_x + _fontXSize, cursor_y + _fontYSize);
}

void Input::hideCursor()
{
    word fcolor = myLCD->getColor();
    myLCD->setColor(myLCD->getBackColor());
    showCursor();
    myLCD->setColor(fcolor);
}

time_t Input::getTimeValue()
{
    int Hour, Min, Sec;
    Hour = atoi(&value[0]);
    Min = atoi(&value[3]);
    Sec = atoi(&value[6]);
//    if (sscanf(value, "%d:%d:%d", &Hour, &Min, &Sec) == 3)
    return Hour * SECS_PER_HOUR + Min * SECS_PER_MIN + Sec;
//    return 0;
}

int Input::getIntValue()
{
    return atoi(value);
}

float Input::getFloatValue()
{
    return atof(value);
}

// void getTime(char ch)
// {
//     int Hour, Min, Sec;
//     switch (ch)
//     {
//     case KEY_NONE:
//         return;

//     case KEY_U_SD:
//         break;

//     case KEY_ARROW:
//     {
//         if (next_ch == 0)
//             return;
//         next_ch--;
//         if (_time[next_ch] == ':')
//             next_ch--;
//         _time[next_ch] = '0';
//         return;
//     }
//     case KEY_0:
//     case KEY_1:
//     case KEY_2:
//     case KEY_3:
//     case KEY_4:
//     case KEY_5:
//     case KEY_6:
//     case KEY_7:
//     case KEY_8:
//     case KEY_9:
//     {
//         _time[next_ch] = ch;
//         next_ch++;
//         if (next_ch > 7)
//             break;
//         if (_time[next_ch] == ':')
//             next_ch++;
//         return;
//     }
//     default:
//         return;
//     }
//     if (sscanf(_time, "%d:%d:%d", &Hour, &Min, &Sec) == 3)
//         setTime(Hour, Min, Sec, day(), month(), year());
//     _ir.setCallback(NULL);
//     _display.setCurrentScreen(DisplayProcess::MAIN_MENU);
// }
