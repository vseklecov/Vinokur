#ifndef __INPUT_H__
#define __INPUT_H__

#include <UTFT.h>
#include <Time.h>

class Input
{
  public:
    Input(UTFT *screen, int x, int y, char *_label, char *_value, char _delimiter, char _filler)
        : myLCD(screen), left(x), top(y), label(_label), value(_value)
    {
        delimiter = _delimiter;
        filler = _filler;
        cursor = 0;
        _fontXSize = screen->getFontXsize();
        _fontYSize = screen->getFontYsize();
    }

    void draw();
    boolean add_char(char ch);
    void remove_char(void);
    char *getValue() { return value; };
    time_t getTimeValue();
    int getIntValue();
    float getFloatValue();

  private:
    void showCursor();
    void hideCursor();

    UTFT *myLCD;
    int left;
    int top;
    byte _fontXSize;
    byte _fontYSize;
    char *label;
    char *value;
    char filler;
    char delimiter;
    int cursor;
    int cursor_x;
    int cursor_y;
};

#endif