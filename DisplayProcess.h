#ifndef __DISPLAY_PROCESS_H__
#define __DISPLAY_PROCESS_H__

#include <ProcessScheduler.h>
#include <UTFT.h>

#include "Input.h"

class DisplayProcess : public Process
{
public:
  enum SCREENS
  {
    SPLASH = 0,
    MAIN_MENU,
    PARAMETERS,
    DISTIL_MASH,
    FRACTION,
  };

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
  void setCurrentScreen(enum SCREENS scr) { curScreen = scr; }
  void showCursor(int, int, word);

  Input *activeInput;

private:
  UTFT *_myLCD;
  byte _fontXSize = 0;
  byte _fontYSize = 0;

  enum SCREENS prevScreen;
  enum SCREENS curScreen;

  String utf8rus(String);
};

#endif