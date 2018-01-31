#ifndef __DISPLAY_PROCESS_H__
#define __DISPLAY_PROCESS_H__

#include <avr/pgmspace.h>
#include <ProcessScheduler.h>
#include <TimeLib.h>
#include <UTFT.h>

enum SCREENS
{
  SPLASH = 0,
  MAIN_MENU,
  DISTIL_MASH,
  FRACTION,
};

class DisplayProcess: public Process
{
  public:
    DisplayProcess(Scheduler &manager, ProcPriority pr, unsigned int period, UTFT *LCD)
      : Process(manager, pr, period)
      {
        _myLCD = LCD;
        prevScreen = SPLASH;
        curScreen = SPLASH;
      }

    void setup(void);
    void service(void);

    void changeScreen(void);
    inline void setCurrentScreen(enum SCREENS scr) { curScreen = scr;}
    void showCursor(int, int, word);
     
  private:
    UTFT *_myLCD;
    byte _fontXSize=0;
    byte _fontYSize=0;

    enum SCREENS prevScreen;
    enum SCREENS curScreen;    

    String utf8rus(String);
};


class Button
{
  public:
    Button(uint16_t x, uint16_t y, uint16_t width, uint16_t height, char *_label);
    Button(uint16_t x, uint16_t y, char *_label);
    Button(uint16_t x, uint16_t y, String _label);

    void draw();
    boolean getState() {
      return state;
    };
    void setState(boolean st) {
      state=st;
    };
    void setTextFont(uint8_t* font);
    void setButtonColors(word atxt, word iatxt, word brd, word brdhi, word back);
  
  private:
    uint16_t pos_x;
    uint16_t pos_y;
    uint16_t width;
    uint16_t height;
    char *label;
    boolean state;
    word _color_text, _color_text_inactive, _color_background, _color_border, _color_hilite;
    uint8_t *_font_text;
    byte _fontXSize;
    byte _fontYSize;
};

#endif
