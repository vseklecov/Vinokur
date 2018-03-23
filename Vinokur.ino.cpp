# 1 "c:\\users\\vsekle~1\\appdata\\local\\temp\\tmpnuwlyg"
#include <Arduino.h>
# 1 "C:/Users/vseklecov/Documents/PlatformIO/Projects/Vinokur/src/Vinokur.ino"
#include "Vinokur.h"
#include "Obmen.h"



#include <ProcessScheduler.h>
Scheduler sched;

#include "SensorsProcess.h"
OneWire ds(ONE_WIRE_BUS);
SensorsProcess _sensors(sched, HIGH_PRIORITY, 1000, &ds);

#include "DistillationProcess.h"
DistillMashProcess _distilMash(sched, HIGH_PRIORITY, 500, &_sensors);
FractioningProcess _refluxStill(sched, HIGH_PRIORITY, 500, &_sensors);

#ifdef _TFT_
#include "DisplayProcess.h"
UTFT myLCD(CTE32HR, TFT_RS, TFT_WR, TFT_CS, TFT_RST);
DisplayProcess _display(sched, MEDIUM_PRIORITY, 1000, &myLCD);
#endif

#ifdef _LOG_
#include "LoggerProcess.h"
LoggerProcess _logger(sched, MEDIUM_PRIORITY, 10000);
#endif

#ifdef _KEYPAD_
#include "KeypadProcess.h"
KeypadProcess _keypad(sched, HIGH_PRIORITY, 500);
#endif

#ifdef _IR_
#include "IRProcess.h"
IRProcess _ir(sched, HIGH_PRIORITY, 500, IR_PIN);
#endif

#ifdef _RTC_
#include <Wire.h>
#include <DS1307RTC.h>
#endif

#include <SPI.h>

#ifdef _BUZ_
#include "Buzzer.h"
Buzzer buzzer(sched, HIGH_PRIORITY, 50, BUZZER);
#endif

unsigned long cMillis;
void setup();
void loop();
void getTime(char ch);
void getMode(char ch);
void getTimeNew(char ch);
void controlProcess(char ch);
float alcSteam(float temp);
float alcLiq(float temp);
String two(int num);
void initObmen();
void serialEvent();
time_t requestSync();
#line 57 "C:/Users/vseklecov/Documents/PlatformIO/Projects/Vinokur/src/Vinokur.ino"
void setup()
{

  initObmen();

  _sensors.add(true);
#ifndef _IR_
  _distilMash.add(true);
#else
  _distilMash.add(false);
  _refluxStill.add(false);
#endif

#ifdef _TFT_
  _display.add(true);
#endif

#ifdef _LOG_
  _logger.add(true);
#endif

#ifdef _KEYPAD_
  _keypad.add(true);
#endif

#ifdef _IR_
  _ir.add(true);
#endif

#ifdef _RTC_
  setSyncProvider(RTC.get);

#ifdef _DEBUG_
  if (timeStatus() != timeSet)
    Serial.println("Unable to sync with the RTC");
  else
    Serial.println("RTC has set the system time");
#else
  timeStatus();
#endif

#else
  setTime(DEFAULT_TIME);
  requestSync();
  setSyncProvider(requestSync);
#endif

#ifdef _BUZ_
  buzzer.add(false);
#endif
}

void loop()
{

#ifdef _LM35_
  _sensors.updateLM35();
#endif

  sched.run();
}
# 1 "C:/Users/vseklecov/Documents/PlatformIO/Projects/Vinokur/src/Button.ino"
#include <Arduino.h>

#include <UTFT.h>



extern UTFT myLCD;



#include "Button.h"



Button::Button(uint16_t x, uint16_t y, uint16_t w, uint16_t h, char *_label)

{

  pos_x = x;

  pos_y = y;

  width = w;

  height = h;

  label = _label;

  state = false;

}



Button::Button(uint16_t x, uint16_t y, char *_label)

{

  _font_text = myLCD.getFont();

  _fontXSize = myLCD.getFontXsize();

  _fontYSize = myLCD.getFontYsize();



  pos_x = x;

  pos_y = y;

  label = _label;

  width = (strlen(label) + 1) * _fontXSize;

  height = _fontYSize * 2;

  state = false;

  _color_text = VGA_AQUA;

  _color_text_inactive = VGA_GRAY;

  _color_background = VGA_BLUE;

  _color_border = VGA_WHITE;

  _color_hilite = VGA_RED;

}



Button::Button(uint16_t x, uint16_t y, String _label)

{

  _font_text = myLCD.getFont();

  _fontXSize = myLCD.getFontXsize();

  _fontYSize = myLCD.getFontYsize();

  pos_x = x;

  pos_y = y;

  label = (char *)malloc(_label.length() + 1);

  _label.toCharArray(label, _label.length() + 1);

  width = (strlen(label) + 1) * _fontXSize;

  height = _fontYSize * 2;

  state = false;

  _color_text = VGA_AQUA;

  _color_text_inactive = VGA_GRAY;

  _color_background = VGA_BLUE;

  _color_border = VGA_WHITE;

  _color_hilite = VGA_RED;

}



void Button::draw()

{



  uint8_t *_current_font = myLCD.getFont();

  ;

  word _current_color = myLCD.getColor();

  word _current_back = myLCD.getBackColor();



  uint16_t text_x = pos_x + width / 2 - (strlen(label) * _fontXSize) / 2;

  uint16_t text_y = pos_y + height / 2 - _fontYSize / 2;

  myLCD.setFont(_font_text);

  if (state)

  {

    myLCD.setColor(_color_background);

    myLCD.fillRoundRect(pos_x, pos_y, pos_x + width, pos_y + height);

    myLCD.setColor(_color_border);

    myLCD.drawRoundRect(pos_x, pos_y, pos_x + width, pos_y + height);

    myLCD.setColor(_color_text);

    myLCD.setBackColor(_color_background);

    myLCD.print(label, text_x, text_y);

  }

  else

  {

    myLCD.setColor(_current_back);

    myLCD.fillRoundRect(pos_x, pos_y, pos_x + width, pos_y + height);

    myLCD.setColor(_color_border);

    myLCD.drawRoundRect(pos_x, pos_y, pos_x + width, pos_y + height);

    myLCD.setColor(_color_text_inactive);

    myLCD.setBackColor(_current_back);

    myLCD.print(label, text_x, text_y);

  }

  myLCD.setFont(_current_font);

  myLCD.setColor(_current_color);

  myLCD.setBackColor(_current_back);

}



void Button::setTextFont(uint8_t *font)

{

  uint8_t *_current_font = myLCD.getFont();

  ;

  _font_text = font;

  myLCD.setFont(_font_text);

  _fontXSize = myLCD.getFontXsize();

  _fontYSize = myLCD.getFontYsize();

  myLCD.setFont(_current_font);

}



void Button::setButtonColors(word atxt, word iatxt, word brd, word brdhi, word back)

{

  _color_text = atxt;

  _color_text_inactive = iatxt;

  _color_background = back;

  _color_border = brd;

  _color_hilite = brdhi;

}
# 1 "C:/Users/vseklecov/Documents/PlatformIO/Projects/Vinokur/src/Buzzer.ino"
#include <Arduino.h>



#include "Buzzer.h"



Song S;



int s_notes[] = {NOTE_C4, NOTE_G3};

int s_beats[] = {250, 125};



void Buzzer::setup()

{

    pinMode(_pin, OUTPUT);

    S.length = 1;

    S.notes = s_notes;

    S.beats = s_beats;
# 71 "C:/Users/vseklecov/Documents/PlatformIO/Projects/Vinokur/src/Buzzer.ino"
}



void Buzzer::playSong(Song *_song)

{

    if (_playSong == NULL)

    {

        _playSong = _song;

        _curNote = 0;

        _delay = 0;

        enable();

    }

}



void Buzzer::service()

{

    if (_delay < millis())

    {

        if (_playSong != NULL)

        {

            if (_curNote < _playSong->length)

            {

                if (_playSong->notes[_curNote] == 0)

                {

                    _delay = millis() + _playSong->beats[_curNote];

                }

                else

                {

                    tone(_pin, _playSong->notes[_curNote], _playSong->beats[_curNote]);

                    _delay = millis() + _playSong->beats[_curNote] * 1.3;

                }

                _curNote++;

            }

            else

            {

                noTone(_pin);

                _playSong = NULL;

                _curNote = 0;

                disable();

            }

        }

    }

}
# 1 "C:/Users/vseklecov/Documents/PlatformIO/Projects/Vinokur/src/DisplayProcess.ino"
#include <Arduino.h>
#include <avr/pgmspace.h>
#include <TimeLib.h>

#include "DisplayProcess.h"
#include "Functions.h"
#include "Keys.h"
#include "Input.h"

#ifdef _BUZ_
#include "Buzzer.h"
extern Song S;
#endif

#include <UTFT.h>
extern UTFT myLCD;

#include "DistillationProcess.h"
extern DistillMashProcess _distilMash;
extern FractioningProcess _refluxStill;

extern uint8_t Tahoma1251[];

#include "Button.h"

Button *cool;
Button *heating;
Button *finish;
Button *alarm;
Input *inputTime;

byte next_ch = 0;
char _time[] = {'0', '0', ':', '0', '0', ':', '0', '0', '\0'};

void DisplayProcess::setup()
{


  _myLCD->InitLCD();
  _myLCD->clrScr();
  _myLCD->setFont(Tahoma1251);
  _fontXSize = _myLCD->getFontXsize();
  _fontYSize = _myLCD->getFontYsize();

  _myLCD->setColor(VGA_AQUA);
  _myLCD->print((char *)"ВИНОКУР", CENTER, _fontYSize * 2);
  _myLCD->print((char *)"версия 0.17", CENTER, _fontYSize * 4);

  heating = new Button(_fontXSize / 2, _fontYSize * 5, (char *)"Нагр");
  cool = new Button(_fontXSize * 8, _fontYSize * 5, (char *)"Охл");
  finish = new Button(_fontXSize * 12.5, _fontYSize * 5, (char *)"Конец");

#ifdef _IR_


  _myLCD->print((char *)"Время:", 0, _fontYSize * 6);
  _myLCD->print(_time, _fontXSize * 8, _fontYSize * 6);
  _ir.setCallback(getTime);
#else
  curScreen = DISTIL_MASH;
  delay(1000);
#endif
}

void DisplayProcess::changeScreen()
{

  _myLCD->clrScr();
  switch (curScreen)
  {
  case MAIN_MENU:
    _myLCD->setColor(VGA_AQUA);
    _myLCD->print((char *)"Режим работы:", CENTER, 0);
    _myLCD->print((char *)"0. Параметры", 0, _fontYSize);
    _myLCD->print("1. " + _distilMash.getHeader(), 0, _fontYSize * 2);
    _myLCD->print("2. " + _refluxStill.getHeader(), 0, _fontYSize * 3);
#ifdef _IR_
    _ir.setCallback(getMode);
#endif
    break;
  case PARAMETERS:
  {
    Input *param1 = new Input(_myLCD, 0, _fontYSize, (char *)"1234567890:", (char *)"000", '.', '0');
    param1->draw();
  }
  break;
  case DISTIL_MASH:
    _myLCD->setColor(VGA_AQUA);
    _myLCD->print(_distilMash.getHeader(), CENTER, 0);
    _myLCD->print((char *)"Куб", _fontXSize, _fontYSize * 2);
    _myLCD->print((char *)"Деф", _fontXSize, _fontYSize * 3);
#ifdef _LM35_
    _myLCD->print((char *)"TST", _fontXSize, _fontYSize * 10);
#endif
    _myLCD->printChar('%', _fontXSize * 16, _fontYSize * 2);
    _myLCD->printChar('%', _fontXSize * 16, _fontYSize * 3);
#ifdef _PRESS_
    _myLCD->print((char *)"мм", _fontXSize * 18, _fontYSize * 9);
#endif
    cool->draw();
    heating->draw();
    finish->draw();
    break;
  case FRACTION:
    _myLCD->setColor(VGA_AQUA);
    _myLCD->print(_refluxStill.getHeader(), CENTER, 0);
    _myLCD->print((char *)"Куб", _fontXSize, _fontYSize * 2);
    _myLCD->print((char *)"Деф", _fontXSize, _fontYSize * 3);
    _myLCD->printChar('%', _fontXSize * 16, _fontYSize * 2);
    _myLCD->printChar('%', _fontXSize * 16, _fontYSize * 3);
#ifdef _PRESS_
    _myLCD->print((char *)"мм", _fontXSize * 18, _fontYSize * 9);
#endif
    cool->draw();
    heating->draw();
    finish->draw();
    break;

  default:
    break;
  }
  prevScreen = curScreen;
}

void DisplayProcess::service()
{
#ifdef _DEBUG_
  unsigned long b_t = millis();
#endif

  if (curScreen != prevScreen)
  {
    changeScreen();
  }
  switch (curScreen)
  {
  case SPLASH:

    if (next_ch != 0)
    {
      if (_time[next_ch - 1] == ':')
        showCursor(_fontXSize * (6 + next_ch), _fontYSize * 6, _myLCD->getBackColor());
      else
        showCursor(_fontXSize * (7 + next_ch), _fontYSize * 6, _myLCD->getBackColor());
    }
    showCursor(_fontXSize * (8 + next_ch), _fontYSize * 6, _myLCD->getColor());
    break;
  case MAIN_MENU:
    break;
  case DISTIL_MASH:
    _myLCD->setColor(VGA_AQUA);
    _myLCD->print(_distilMash.getStateString(), 0, _fontYSize);
#ifdef _DEBUG_
    Serial.println("Display Process: State string " + String(millis() - b_t));
    b_t = millis();
#endif
    _myLCD->print(formatTime(2, now() - _distilMash.getTimeBegin()), _fontXSize * 7, _fontYSize);
    _myLCD->print(formatTime(3, 0), CENTER, _fontYSize * 9);
#ifdef _PRESS_
    _myLCD->printNumI(round(_sensors.getPressure() * 7.5E-3), _fontXSize * 15, _fontYSize * 9, 3);
#endif
    if (_distilMash.boiling_kub > 0)
      _myLCD->printNumF(_distilMash.boiling_kub, 1, 0, _fontYSize * 9);

#ifdef _DEBUG_
    Serial.println("Display Process: format time string " + String(millis() - b_t));
    b_t = millis();
#endif
    _myLCD->printNumF(_sensors.getThempKub(), 1, _fontXSize * 5, _fontYSize * 2, '.', 5);
#ifdef _DEBUG_
    Serial.println("Display Process: Temp kub string " + String(millis() - b_t));
    b_t = millis();
#endif
    _myLCD->printNumF(_sensors.getThempDef(), 1, _fontXSize * 5, _fontYSize * 3, '.', 5);
#ifdef _DEBUG_
    Serial.println("Display Process: Temp def string " + String(millis() - b_t));
    b_t = millis();
#endif
#ifdef _LM35_
    _myLCD->printNumF(_sensors.getTD(), 1, _fontXSize * 5, _fontYSize * 10, '.', 5);
#endif
    _myLCD->printNumF(alcLiq(_sensors.getThempKub()), 1, _fontXSize * 12, _fontYSize * 2, '.', 4);
#ifdef _DEBUG_
    Serial.println("Display Process: Alc kub string " + String(millis() - b_t));
    b_t = millis();
#endif
    _myLCD->printNumF(alcSteam(_sensors.getThempDef()), 1, _fontXSize * 12, _fontYSize * 3, '.', 4);
#ifdef _DEBUG_
    Serial.println("Display Process: Alc def string " + String(millis() - b_t));
    b_t = millis();
#endif

    if (_distilMash.getHeating() != heating->getState())
    {
      heating->setState(_distilMash.getHeating());
      heating->draw();
    }
    if (_distilMash.getCooling() != cool->getState())
    {
      cool->setState(_distilMash.getCooling());
      cool->draw();
#ifdef _BUZ_
      if (cool->getState())
        buzzer.playSong(&S);
#endif
    }
    if ((_distilMash.getState() == STATE_END) != finish->getState())
    {
      finish->setState(_distilMash.getState() == STATE_END);
      finish->draw();
#ifdef _BUZ_
      if (finish->getState())
        buzzer.playSong(&S);
#endif
    }
    break;

  case FRACTION:
    _myLCD->setColor(VGA_AQUA);
    _myLCD->print(utf8rus(_refluxStill.getStateString()), 0, _fontYSize);
    _myLCD->print(formatTime(2, now() - _refluxStill.getTimeBegin()), _fontXSize * 7, _fontYSize);
    _myLCD->print(formatTime(3, 0), CENTER, _fontYSize * 9);
#ifdef _PRESS_
    _myLCD->printNumI(round(_sensors.getPressure() * 7.5E-3), _fontXSize * 15, _fontYSize * 9, 3);
#endif
    if (_refluxStill.boiling_kub > 0)
      _myLCD->printNumF(_refluxStill.boiling_kub, 1, 0, _fontYSize * 9);
    _myLCD->printNumF(_sensors.getThempKub(), 1, _fontXSize * 5, _fontYSize * 2, '.', 5);
    _myLCD->printNumF(_sensors.getThempDef(), 1, _fontXSize * 5, _fontYSize * 3, '.', 5);
    _myLCD->printNumF(alcLiq(_sensors.getThempKub()), 1, _fontXSize * 12, _fontYSize * 2, '.', 4);
    _myLCD->printNumF(alcSteam(_sensors.getThempDef()), 1, _fontXSize * 12, _fontYSize * 3, '.', 4);


    if (_refluxStill.getHeating() != heating->getState())
    {
      heating->setState(_refluxStill.getHeating());
      heating->draw();
    }
    if (_refluxStill.getCooling() != cool->getState())
    {
      cool->setState(_refluxStill.getCooling());
      cool->draw();
#ifdef _BUZ_
      if (cool->getState())
        buzzer.playSong(&S);
#endif
    }
    if ((_refluxStill.getState() == STATE_END) != finish->getState())
    {
      finish->setState(_refluxStill.getState() == STATE_END);
      finish->draw();
#ifdef _BUZ_
      if (finish->getState())
        buzzer.playSong(&S);
#endif
    }
    break;

  default:
    break;
  }

#ifdef _DEBUG_
  Serial.println("Display Process: " + String(millis() - b_t));
#endif
}


String DisplayProcess::utf8rus(String source)
{
  int i = 0;
  int k = source.length();
  String target;
  unsigned char n;
  char m[2] = {'0', '\0'};

  while (i < k)
  {
    n = source[i++];

    if (n >= 0xC0)
    {
      switch (n)
      {
      case 0xD0:
      {
        n = source[i++];
        if (n == 0x81)
        {
          n = 0x40;
          break;
        }
        if (n >= 0x90 && n <= 0xBF)
          n = n - 0x4E;
        break;
      }
      case 0xD1:
      {
        n = source[i++];
        if (n == 0x91)
        {
          n = 0x41;
          break;
        }
        if (n >= 0x80 && n <= 0x8F)
          n = n - 0x0E;
        break;
      }
      }
    }
    m[0] = n;
    target = target + String(m);
  }
  return target;
}

void DisplayProcess::showCursor(int x, int y, word color)
{
  word fcolor = _myLCD->getColor();
  _myLCD->setColor(color);
  _myLCD->fillRect(x, y + _fontYSize - 1, x + _fontXSize, y + _fontYSize);
  _myLCD->setColor(fcolor);
}

#ifdef _IR_
void getTime(char ch)
{
  int Hour, Min, Sec;
  unsigned int cursor_x = myLCD.getFontXsize();
  unsigned int cursor_y = myLCD.getFontYsize() * 6;

  switch (ch)
  {
  case KEY_NONE:
    return;

  case KEY_U_SD:
    break;

  case KEY_ARROW:
  {
    if (next_ch == 0)
      return;
    next_ch--;
    if (_time[next_ch] == ':')
      next_ch--;
    _time[next_ch] = '0';
    cursor_x = cursor_x * (8 + next_ch);
    myLCD.printChar('0', cursor_x, cursor_y);

    return;
  }
  case KEY_0:
  case KEY_1:
  case KEY_2:
  case KEY_3:
  case KEY_4:
  case KEY_5:
  case KEY_6:
  case KEY_7:
  case KEY_8:
  case KEY_9:
  {
    _time[next_ch] = ch;
    cursor_x = cursor_x * (8 + next_ch);
    myLCD.printChar(ch, cursor_x, cursor_y);

    next_ch++;
    if (next_ch > 7)
      break;
    if (_time[next_ch] == ':')
      next_ch++;
    return;
  }
  default:
    return;
  }
  if (sscanf(_time, "%d:%d:%d", &Hour, &Min, &Sec) == 3)
    setTime(Hour, Min, Sec, day(), month(), year());
  _ir.setCallback(NULL);
  _display.setCurrentScreen(DisplayProcess::MAIN_MENU);
}

void getMode(char ch)
{
  switch (ch)
  {
  case KEY_0:
    _ir.setCallback(NULL);
    _display.setCurrentScreen(DisplayProcess::PARAMETERS);
  case KEY_1:
    _ir.setCallback(controlProcess);
    _display.setCurrentScreen(DisplayProcess::DISTIL_MASH);
    _distilMash.setState(STATE_STOPS);
    _distilMash.enable();
    break;
  case KEY_2:
    _ir.setCallback(controlProcess);
    _display.setCurrentScreen(DisplayProcess::FRACTION);
    _refluxStill.setState(STATE_STOPS);
    _refluxStill.enable();
    break;
  default:
    break;
  }
}

void getTimeNew(char ch)
{
  if (_display.activeInput == NULL)
    return;
  Serial.println(ch);
  switch (ch)
  {
  case KEY_NONE:
    return;

  case KEY_U_SD:
    break;

  case KEY_ARROW:
    _display.activeInput->remove_char();
    break;
  case KEY_0:
  case KEY_1:
  case KEY_2:
  case KEY_3:
  case KEY_4:
  case KEY_5:
  case KEY_6:
  case KEY_7:
  case KEY_8:
  case KEY_9:
    if (_display.activeInput->add_char(ch))
      break;
    else
      return;
  default:
    return;
  }
  setTime(_display.activeInput->getTimeValue());
  _ir.setCallback(NULL);
  _display.activeInput = NULL;
  _display.setCurrentScreen(DisplayProcess::MAIN_MENU);
}

#endif
# 1 "C:/Users/vseklecov/Documents/PlatformIO/Projects/Vinokur/src/DistillationProcess.ino"
#ifdef _BUZ_
#include "Buzzer.h"
#endif

#include "DistillationProcess.h"
#include "DisplayProcess.h"

void DistillationProcess::setup()
{
  setState(STATE_STOPS);
  setCooling(0);
  setHeating(0);
  time_begin = now();
  temp_boiling = 0.0;
}

void DistillationProcess::service()
{
#ifdef _DEBUG_
  unsigned long b_t = millis();
#endif

  switch (_state)
  {
  case STATE_STOPS:
    if ((_sp->getThempKub() - _sp->getThempDef()) > 5)
      setNextState();
    break;
  case STATE_HEAT:
    setHeating(100);
    if (_sp->getThempKub() > _temp_cool_on || _sp->getThempDef() > _temp_def_cool_on)
    {
      setCooling(100);
    }
    if (boiling_kub > 0)
    {
      setNextState();
    }
    break;
  case STATE_DISTIL:
    setHeating(100);
    setCooling(100);
    if (_sp->getThempKub() > _temp_distil_off || _sp->getThempDef() > _temp_def_distil_off)
    {
      setNextState();
    }
    break;
  case STATE_END:
    if (getHeating())
    {
      setHeating(0);
      _delay = millis() + 20000;
    }
    else if (millis() > _delay)
    {
      setCooling(0);
      disable();
    }
    break;
  case STATE_CANCEL:
    setHeating(0);
    setCooling(0);
    disable();
    break;
  default:
    break;
  };

  if (boiling_kub == 0.0 && _state == STATE_HEAT && getCooling())
  {
    if (temp_boiling == 0.0)
    {
      temp_boiling = _sp->getThempKub();
      time_boiling = millis();
    }
    if ((millis() - time_boiling) > 20000)
    {
      if (abs(_sp->getThempKub() - temp_boiling) < 0.15)
        boiling_kub = temp_boiling;
      else
      {
        temp_boiling = _sp->getThempKub();
        time_boiling = millis();
      }
    }
  };

#ifdef _DEBUG_
  Serial.println("Distillation Process: " + String(millis() - b_t));
#endif
}

void DistillationProcess::setState(states state)
{
  if (state == STATE_HEAT)
    time_begin = now();
  _state = state;
}

void DistillationProcess::setNextState()
{
  switch (_state)
  {
  case STATE_STOPS:
    setState(STATE_HEAT);
    break;
  case STATE_HEAT:
    setState(STATE_DISTIL);
    break;
  case STATE_DISTIL:
    setState(STATE_END);
    break;
  case STATE_END:
  default:
    break;
  }
}

void DistillationProcess::setPrevState()
{
  switch (_state)
  {
  case STATE_STOPS:
    break;
  case STATE_HEAT:
    setState(STATE_STOPS);
    break;
  case STATE_DISTIL:
    setState(STATE_HEAT);
    break;
  case STATE_END:
    setState(STATE_DISTIL);
    break;
  default:
    break;
  }
}


void DistillationProcess::setCooling(int procent)
{
  _cooling = procent;
}


void DistillationProcess::setHeating(int procent)
{
  _heating = procent;
}

String DistillationProcess::getStateString()
{
  switch (_state)
  {
  case STATE_STOPS:
    return "Стоп  ";
  case STATE_HEAT:
    return "Нагрев";
  case STATE_DISTIL:
    return "Отбор ";
  case STATE_END:
    return "Конец ";
  case STATE_CANCEL:
    return "Прервн";
  case STATE_EQUIL:
    return "НаСебя";
  case STATE_HEAD:
    return "Головы";
  case STATE_HEART:
    return "Тело  ";
  case STATE_TAILS:
    return "Хвосты";
  default:
    break;
  };
  return "";
}

void DistillationProcess::readDataEEPROM()
{
  if (EEPROM[0] == 255)
  {
    _temp_cool_on = 75.0;
    _temp_def_cool_on = 50.0;
    _temp_distil_off = 98.0;
    _temp_def_distil_off = 99.1;
    writeDataEEPROM();
  }
  else
  {
    int temp[4];
    EEPROM.get(2, temp);
    _temp_cool_on = temp[0] / 10;
    _temp_def_cool_on = temp[1] / 10;
    _temp_distil_off = temp[2] / 10;
    _temp_def_distil_off = temp[3] / 10;
  }
}

void DistillationProcess::writeDataEEPROM()
{
  int temp[4];
  byte hi, lo;
  temp[0] = int(_temp_cool_on * 10);
  temp[1] = int(_temp_def_cool_on * 10);
  temp[2] = int(_temp_distil_off * 10);
  temp[3] = int(_temp_def_distil_off * 10);
  if (EEPROM[0] == 255)
  {
    EEPROM[0] = 1;
    EEPROM.put(2, temp);
  }
  else
  {
    for (int i = 0; i < 4; i++)
    {
      hi = highByte(temp[i]);
      lo = lowByte(temp[i]);
      EEPROM.update(2 + i * 2, hi);
      EEPROM.update(3 + i * 2, lo);
    }
  }
}

void FractioningProcess::service()
{
  switch (_state)
  {
  case STATE_HEAT:
    setHeating(100);
    if (_sp->getThempKub() > _temp_cool_on || _sp->getThempDef() > _temp_def_cool_on)
    {
      setCooling(100);
    }
    if (boiling_kub > 0)
    {
      setNextState();
    }
    break;
  case STATE_EQUIL:
    setHeating(40);
    setCooling(100);
    if (_delay == 0)
    {
      _delay = _equilibrate_time;
      _delay = _delay * 1000 + millis();
    }
    else if (millis() > _delay)
    {
      _delay = 0;
      setNextState();
    }
    break;
  case STATE_HEAD:
    setHeating(40);
    setCooling(100);
    break;
  case STATE_HEART:
    setHeating(80);
    setCooling(100);
    if (_sp->getThempKub() > _temp_heart_off || _sp->getThempDef() > _temp_def_heart_off)
      setNextState();
    break;
  case STATE_TAILS:
    setHeating(100);
    setCooling(100);
    if (_sp->getThempKub() > _temp_distil_off || _sp->getThempDef() > _temp_def_distil_off)
    {
      setNextState();
    }
    break;
  default:
    DistillationProcess::service();
    return;
  };

  if (boiling_kub == 0.0 && _state == STATE_HEAT && getCooling())
  {
    if (temp_boiling == 0.0)
    {
      temp_boiling = _sp->getThempKub();
      time_boiling = millis();
    }
    if ((millis() - time_boiling) > 20000)
    {
      if (abs(_sp->getThempKub() - temp_boiling) < 0.1)
        boiling_kub = temp_boiling;
      else
      {
        temp_boiling = _sp->getThempKub();
        time_boiling = millis();
      }
    }
  };
}

void FractioningProcess::setNextState()
{
#ifdef _BUZ_
  buzzer.playSong(&S);
#endif
  switch (_state)
  {
  case STATE_HEAT:
    setState(STATE_EQUIL);
    break;
  case STATE_EQUIL:
    setState(STATE_HEAD);
    break;
  case STATE_HEAD:
    setState(STATE_HEART);
    break;
  case STATE_HEART:
    setState(STATE_TAILS);
    break;
  case STATE_TAILS:
    setState(STATE_END);
    break;
  default:
    DistillationProcess::setNextState();
    break;
  }
}

void FractioningProcess::setPrevState()
{
  switch (_state)
  {
  case STATE_EQUIL:
    setState(STATE_HEAT);
    break;
  case STATE_HEAD:
    setState(STATE_EQUIL);
    break;
  case STATE_HEART:
    setState(STATE_HEAD);
    break;
  case STATE_TAILS:
    setState(STATE_HEART);
    break;
  case STATE_END:
    setState(STATE_TAILS);
    break;
  default:
    DistillationProcess::setPrevState();
    break;
  }
}

void FractioningProcess::readDataEEPROM()
{
  if (EEPROM[0] == 255)
    DistillationProcess::readDataEEPROM();
  if (EEPROM[0] == 1)
  {
    _temp_cool_on = 75.0;
    _temp_def_cool_on = 50.0;
    _temp_distil_off = 98.0;
    _temp_def_distil_off = 99.1;
    _equilibrate_time = 1200;
    _temp_heart_off = 92.0;
    _temp_def_heart_off = 80.0;
    writeDataEEPROM();
  }
  else
  {
    int temp[7];
    EEPROM.get(10, temp);
    _temp_cool_on = temp[0] / 10;
    _temp_def_cool_on = temp[1] / 10;
    _temp_distil_off = temp[2] / 10;
    _temp_def_distil_off = temp[3] / 10;
    _equilibrate_time = temp[4];
    _temp_heart_off = temp[5] / 10;
    _temp_def_heart_off = temp[6] / 10;
  }
}

void FractioningProcess::writeDataEEPROM()
{
  int temp[7];
  byte hi, lo;
  temp[0] = int(_temp_cool_on * 10);
  temp[1] = int(_temp_def_cool_on * 10);
  temp[2] = int(_temp_distil_off * 10);
  temp[3] = int(_temp_def_distil_off * 10);
  temp[4] = _equilibrate_time;
  temp[5] = int(_temp_heart_off * 10);
  temp[6] = int(_temp_def_heart_off * 10);
  if (EEPROM[0] == 255)
    return;
  if (EEPROM[0] == 1)
  {
    EEPROM.write(0, 2);
    EEPROM.put(10, temp);
  }
  else
  {
    for (int i = 0; i < 7; i++)
    {
      hi = highByte(temp[i]);
      lo = lowByte(temp[i]);
      EEPROM.update(10 + i * 2, lo);
      EEPROM.update(11 + i * 2, hi);
    }
  }
}

extern DistillMashProcess _distilMash;
extern FractioningProcess _refluxStill;
extern DisplayProcess _display;

void controlProcess(char ch)
{
  if (!_distilMash.isEnabled() && !_refluxStill.isEnabled() && ch == PROCESS_CANCEL)
  {
    _display.setCurrentScreen(DisplayProcess::MAIN_MENU);
    return;
  }
  switch (ch)
  {
  case PROCESS_START:
    if (_distilMash.isEnabled())
    {
      if (_distilMash.getState() == STATE_STOPS)
        _distilMash.setNextState();
    }
    else if (_refluxStill.isEnabled())
    {
      if (_refluxStill.getState() == STATE_STOPS)
        _refluxStill.setNextState();
    }
    break;
  case PROCESS_CANCEL:
    if (_distilMash.isEnabled())
    {
      _distilMash.setState(STATE_CANCEL);
      _distilMash.disable();
    }
    else if (_refluxStill.isEnabled())
    {
      _refluxStill.setState(STATE_CANCEL);
      _refluxStill.disable();
    }
    _display.setCurrentScreen(DisplayProcess::MAIN_MENU);
    break;
  case PROCESS_PREV_STATE:
    if (_distilMash.isEnabled())
    {
      _distilMash.setPrevState();
    }
    else if (_refluxStill.isEnabled())
    {
      _refluxStill.setPrevState();
    }
    break;
  case PROCESS_NEXT_STATE:
    if (_distilMash.isEnabled())
    {
      _distilMash.setNextState();
    }
    else if (_refluxStill.isEnabled())
    {
      _refluxStill.setNextState();
    }
    break;
  case PROCESS_SETTINGS:
    break;
  default:
    break;
  }
}
# 1 "C:/Users/vseklecov/Documents/PlatformIO/Projects/Vinokur/src/Functions.ino"
#include <Arduino.h>
#include <avr/pgmspace.h>
#include <Time.h>

#include "Functions.h"

const float steam[218] PROGMEM = {
    96.3, 95.2, 94.7, 94.2, 93.8, 93.1, 92.7, 92.2,
    91.9, 91.6, 91.2, 90.8, 90.5, 90.1, 89.6, 89.1, 88.4, 88.1,
    87.8, 87.4, 87.2, 87.0, 86.8, 86.3, 86.1, 85.8, 85.6, 85.4,
    85.1, 84.8, 84.7, 84.3, 84.0, 83.8, 83.6, 83.4, 83.3, 83.0,
    82.8, 82.7, 82.5, 82.4, 82.2, 81.8, 81.7, 81.6, 81.4, 81.2,
    81.1, 81.0, 80.8, 80.7, 80.4, 80.3, 80.1, 80.0, 79.6, 79.5,
    79.4, 79.2, 79.1, 78.8, 78.6, 78.5, 78.3, 78.1, 77.7, 77.4,
    77.2, 77.0, 76.8, 76.6, 76.3, 76.1, 75.9, 75.7, 75.5, 75.3,
    75.0, 74.8, 74.5, 74.2, 73.9, 73.6, 73.4, 73.2, 72.9, 72.7,
    72.4, 72.3, 72.1, 71.9, 71.8, 71.4, 71.1, 70.8, 70.3, 69.8,
    69.6, 69.3, 69.0, 68.8, 68.6, 68.5, 68.3, 68.2, 68.0, 67.9,
    67.7, 67.2, 66.6, 66.3, 66.1, 65.8, 65.5, 65.2, 65.1, 64.9,
    64.7, 64.5, 64.3, 64.1, 63.9, 63.7, 63.1, 62.6, 62.0, 61.6,
    61.2, 60.9, 60.5, 60.1, 59.7, 59.4, 59.0, 58.6, 58.3, 57.9,
    57.5, 57.2, 56.8, 56.5, 56.1, 55.8, 55.4, 55.0, 54.6, 54.2,
    53.8, 53.3, 52.9, 52.5, 52.1, 51.6, 51.1, 50.7, 50.2, 49.7,
    49.3, 48.8, 48.3, 47.7, 47.2, 46.6, 46.1, 45.5, 45.0, 44.4,
    43.9, 43.4, 42.9, 42.4, 41.9, 41.5, 41.0, 40.5, 40.0, 39.2,
    38.4, 37.5, 36.7, 35.9, 35.1, 34.3, 33.4, 32.6, 31.7, 30.8,
    29.9, 29.1, 28.2, 27.3, 26.4, 25.5, 24.6, 23.7, 22.7, 21.7,
    20.8, 19.8, 18.8, 17.8, 16.8, 15.8, 14.9, 13.9, 13.0, 12.3,
    11.2, 10.0, 8.9, 7.6, 6.1, 4.8, 3.3, 2.5, 1.6, 0.2
};

const float liq[] PROGMEM = {
    96.1, 94.7, 94.0, 93.3, 92.5, 91.4, 90.7, 89.9,
    89.2, 88.7, 87.9, 87.1, 86.3, 85.5, 84.3, 83.0, 80.9, 80.0,
    78.7, 77.8, 77.0, 76.1, 75.2, 73.3, 72.4, 71.0, 70.1, 69.2,
    67.7, 66.8, 65.8, 63.9, 62.9, 61.9, 60.9, 59.9, 58.9, 57.9,
    56.9, 56.2, 55.5, 54.8, 53.2, 51.7, 51.0, 50.2, 49.4, 48.8,
    48.1, 47.4, 46.9, 46.3, 45.2, 44.2, 43.6, 43.0, 41.4, 40.6,
    40.3, 40.0, 39.7, 38.5, 38.0, 37.4, 36.8, 36.3, 35.1, 34.5,
    34.0, 33.4, 32.8, 32.2, 31.6, 31.2, 30.9, 30.5, 30.0, 29.5,
    29.1, 28.6, 28.1, 27.5, 26.9, 26.5, 26.1, 25.7, 25.3, 24.9,
    24.5, 24.2, 23.9, 23.6, 23.3, 22.9, 22.5, 22.1, 21.5, 20.9,
    20.6, 20.4, 20.1, 19.8, 19.6, 19.4, 19.2, 19.1, 18.9, 18.7,
    18.5, 17.9, 17.3, 17.1, 16.8, 16.6, 16.4, 16.1, 16.0, 15.8,
    15.7, 15.5, 15.4, 15.2, 15.0, 14.9, 14.5, 14.1, 13.7, 13.4,
    13.2, 12.9, 12.7, 12.5, 12.3, 12.1, 11.8, 11.6, 11.4, 11.2,
    11.0, 10.9, 10.7, 10.5, 10.3, 10.2, 10.0, 9.8, 9.7, 9.5,
    9.3, 9.2, 9.0, 8.8, 8.7, 8.5, 8.4, 8.2, 8.1, 7.9,
    7.8, 7.6, 7.4, 7.3, 7.1, 7.0, 6.8, 6.7, 6.5, 6.4,
    6.2, 6.1, 5.9, 5.8, 5.6, 5.5, 5.3, 5.2, 5.0, 4.9,
    4.7, 4.6, 4.5, 4.3, 4.2, 4.0, 3.9, 3.7, 3.6, 3.5,
    3.4, 3.2, 3.1, 3.0, 2.9, 2.7, 2.6, 2.5, 2.4, 2.3,
    2.1, 2.0, 1.9, 1.8, 1.7, 1.6, 1.5, 1.3, 1.2, 1.2,
    1.0, 0.9, 0.8, 0.6, 0.5, 0.4, 0.3, 0.2, 0.1, 0.0
};

float alcSteam(float temp)
{
  if (temp < 78.2 || temp > 99.9)
    return 0.0;
  else
    return pgm_read_float_near(steam + (int)(temp * 10) - 782);
}

float alcLiq(float temp)
{
  if (temp < 78.2 || temp > 99.9)
    return 0.0;
  else
    return pgm_read_float_near(liq + (int)(temp * 10) - 782);
}


String formatTime(byte f = 2, time_t t = 0)
{
  if (t == 0)
    t = now();
  switch (f)
  {
  case 1:
    return String(year(t)) + "-" + two(month(t)) + "-" + two(day(t)) + " " + two(hour(t)) + ":" + two(minute(t)) + ":" + two(second(t));
  case 2:
    return two(hour(t)) + ":" + two(minute(t)) + ":" + two(second(t));
  case 3:
    return two(hour(t)) + ":" + two(minute(t));
  }
  return "";
}

String two(int num)
{
  if (num < 10)
  {
    return "0" + String(num);
  }
  else
  {
    return String(num);
  }
}


char *utf8rusch(const char *source, char *buf)
{
  int i = 0, j = 0;
  int k = strlen(source);
  unsigned char n;

  while (i < k)
  {
    n = source[i++];

    if (n >= 0xC0)
    {
      switch (n)
      {
      case 0xD0:
      {
        n = source[i++];
        if (n == 0x81)
        {
          n = 0x40;
          break;
        }
        if (n >= 0x90 && n <= 0xBF)
          n = n - 0x4E;
        break;
      }
      case 0xD1:
      {
        n = source[i++];
        if (n == 0x91)
        {
          n = 0x41;
          break;
        }
        if (n >= 0x80 && n <= 0x8F)
          n = n - 0x0E;
        break;
      }
      }
    }
    buf[j++] = n;
  }
  buf[j] = '\0';
  return buf;
}
# 1 "C:/Users/vseklecov/Documents/PlatformIO/Projects/Vinokur/src/IRProcess.ino"
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

    case 0xFFA25D:

        return KEY_ON;

    case 0xFF629D:

        return KEY_MODE;

    case 0xFFE21D:

        return KEY_SOUND;

    case 0xFF22DD:

        return KEY_PLAY;

    case 0xFF02FD:

        return KEY_PREV;

    case 0xFFC23D:

        return KEY_NEXT;

    case 0xFFE01F:

        return KEY_EQ;

    case 0xFFA857:

        return KEY_MINUS;

    case 0xFF906F:

        return KEY_PLUS;

    case 0xFF9867:

        return KEY_ARROW;

    case 0xFFB04F:

        return KEY_U_SD;

    case 0xFF6897:

        return KEY_0;

    case 0xFF30CF:

        return KEY_1;

    case 0xFF18E7:

        return KEY_2;

    case 0xFF7A85:

        return KEY_3;

    case 0xFF10EF:

        return KEY_4;

    case 0xFF38C7:

        return KEY_5;

    case 0xFF5AA5:

        return KEY_6;

    case 0xFF42BD:

        return KEY_7;

    case 0xFF4AB5:

        return KEY_8;

    case 0xFF52AD:

        return KEY_9;

    default:

        break;

    }

    return KEY_NONE;

}
# 1 "C:/Users/vseklecov/Documents/PlatformIO/Projects/Vinokur/src/Input.ino"
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



    return Hour * SECS_PER_HOUR + Min * SECS_PER_MIN + Sec;



}



int Input::getIntValue()

{

    return atoi(value);

}



float Input::getFloatValue()

{

    return atof(value);

}
# 1 "C:/Users/vseklecov/Documents/PlatformIO/Projects/Vinokur/src/LoggerProcess.ino"
#include <Arduino.h>



#include "LoggerProcess.h"



#include <SD.h>

#include <TimeLib.h>



#include "SensorsProcess.h"

extern SensorsProcess _sensors;



#include "DistillationProcess.h"

extern DistillMashProcess _distilMash;

extern FractioningProcess _refluxStill;



extern String formatTime(byte, time_t);



void LoggerProcess::setup()

{

    if (!SD.begin(SD_CS))

    {

        disable();

    }

}



void LoggerProcess::service()

{

    DistillationProcess *_still;

#ifdef _DEBUG_

    unsigned long b_t = millis();

#endif



    if (_distilMash.isEnabled())

        _still = &_distilMash;

    else if (_refluxStill.isEnabled())

        _still = &_refluxStill;

    else

    {

        _write_header = false;

        return;

    }



    filenameLog = "distmash.csv";

    File dataFile = SD.open(filenameLog, FILE_WRITE);

    if (dataFile)

    {

        switch (_still->getState())

        {

        case STATE_STOPS:

            if (!_write_header)

            {

                dataFile.println(_still->getHeader());

                _write_header = true;

            }

            break;



        case STATE_CANCEL:

            break;

        default:

        {

            dataFile.print(formatTime(2, 0));

#ifdef _LM35_

            dataFile.print(";" + String(_sensors.getTD()));

#endif

            dataFile.print(";" + String(_sensors.getThempKub()));

            dataFile.print(";" + String(_sensors.getThempDef()));

#ifdef _PRESS_

            dataFile.print(";" + String(_sensors.getPressure()));

#endif

            dataFile.print(";" + _still->getStateString());

            if (_still->boiling_kub > 0.0)

                dataFile.println(";" + String(_still->boiling_kub));

            else

                dataFile.println(";" + String(_still->temp_boiling));

        }

        }

        dataFile.close();

    }



#ifdef _DEBUG_

    Serial.println("Logger Process: " + String(millis() - b_t));

#endif

}
# 1 "C:/Users/vseklecov/Documents/PlatformIO/Projects/Vinokur/src/Obmen.ino"
#include <EEPROM.h>
#include <SPI.h>
#include <SD.h>

#include "Vinokur.h"
#include "Obmen.h"
#include "SensorsProcess.h"

extern SensorsProcess _sensors;

void initObmen()
{
  Serial.begin(BAUDRATE);
}


void serialEvent()
{

  char command = (char)Serial.read();
  switch (command)
  {
  case HELP_HEADER:
    Serial.println("Список команд:");
    Serial.println("\tT\tУстановить время число");
    Serial.println("\tD\tУстановить время строка");
    Serial.println("\tP\tУстановить давление");
    Serial.println("\tS\tПослать файл-лог");
    Serial.println("\tR\tУдалить файл-лог");
    Serial.println("\tL\tПередать список файлов");
    Serial.println("\tG\tПоказать параметры");
    Serial.println("\tH\tПередать список команд");
    break;
  case TIME_HEADER:
  {
    unsigned long pctime = Serial.parseInt();
    if (pctime > DEFAULT_TIME)
    {
      setTime(pctime);
    }
  }
  break;
  case DATE_HEADER:
    break;
  case PRES_HEADER:
    _sensors.setPressure(Serial.parseInt());
    break;
  case PARAMETERS:
    Serial.println("Версия:\t" + String(EEPROM[0]));
    if (EEPROM[0] > 0)
    {
      int temp[4];
      EEPROM.get(2, temp);
      Serial.println("Первичная дистилляция:");
      Serial.println("Охлаждение куб:\t" + String(temp[0] / 10));
      Serial.println("Охлаждение деф:\t" + String(temp[1] / 10));
      Serial.println("Окончание куб:\t" + String(temp[2] / 10));
      Serial.println("Окончание деф:\t" + String(temp[3] / 10));
    }
    if (EEPROM[0] > 1)
    {
      int temp[7];
      EEPROM.get(10, temp);
      Serial.println("Дробная дистилляция:");
      Serial.println("Охлаждение куб:\t" + String(temp[0] / 10));
      Serial.println("Охлаждение деф:\t" + String(temp[1] / 10));
      Serial.println("Окончание куб:\t" + String(temp[2] / 10));
      Serial.println("Окончание деф:\t" + String(temp[3] / 10));
      Serial.println("Время работы на себя:\t" + String(temp[4]));
      Serial.println("Хвосты куб:\t" + String(temp[5] / 10));
      Serial.println("Хвосты деф:\t" + String(temp[6] / 10));
    }
    for (int i = 24; EEPROM[i] != 255; i++)
    {
      Serial.println(String(i) + ":\t" + String(EEPROM[i]));
    }
    break;
  case LIST_SD:
  {
    Sd2Card card;
    SdVolume volume;
    SdFile root;
    card.init(SPI_HALF_SPEED, SD_CS);
    volume.init(card);
    root.openRoot(volume);
    root.ls(LS_R | LS_DATE | LS_SIZE);
  }
  break;
#ifdef _LOG_
  case READ_FILE:
  {
    File dataFile = SD.open("distmash.csv", FILE_READ);
    if (dataFile)
    {
      while (dataFile.available())
      {
        Serial.write(dataFile.read());
      }
      Serial.write(END_OF_FILE);
      dataFile.close();
    }
  }
  break;
  case DEL_FILE:
    if (SD.exists("distmash.csv"))
    {
      File dataFile = SD.open("distmash.csv", O_WRITE | O_TRUNC);
      dataFile.close();
    }
    break;
#endif
  default:
    Serial.println((char)command);
  }
}

#ifndef _RTC_

time_t requestSync()
{

  return 0;
}
#endif
# 1 "C:/Users/vseklecov/Documents/PlatformIO/Projects/Vinokur/src/SensorsProcess.ino"
#include "SensorsProcess.h"

#define TERMOMETR_YELLOW \
  { \
    0x28, 0xFF, 0xB6, 0x97, 0x81, 0x16, 0x05, 0x4C \
  }

#ifdef _LM35_

#define C5V 0.48828125
#define C3V3 0.322265625
#define C1V1 0.107421875
#define C2V56 0.25
#endif

void SensorsProcess::setup()
{
  _sensors.begin();
  if (_sensors.isConnected(kubThermometer))
  {
    _sensors.setResolution(kubThermometer, TEMP_12_BIT);
  }
  if (_sensors.isConnected(defThermometer))
  {
    _sensors.setResolution(defThermometer, TEMP_12_BIT);
  }

  _sensors.setWaitForConversion(false);
  _sensors.requestTemperatures();
  _sensors.setWaitForConversion(true);

#ifdef _LM35_
  analogReference(DEFAULT);
  pinMode(td.pin, INPUT);
#endif

#ifdef _PRESS_
  _bmp280ready = _bmp280.begin();
#ifdef _DEBUG_
  if (!_bmp280ready)
    Serial.println("Датчик BMP280 не подключен");
#endif
#endif
}

void SensorsProcess::service()
{

#ifdef _DEBUG_
  unsigned long b_t = millis();
#endif

#ifdef _PRESS_
  if (_bmp280ready)
    _pressure = _bmp280.readPressure();
#endif

  t_kub = readTemp(kubThermometer);
  t_def = readTemp(defThermometer);

  _sensors.setWaitForConversion(false);
  _sensors.requestTemperatures();
  _sensors.setWaitForConversion(true);

#ifdef _DEBUG_
  Serial.print(String(t_kub) + " " + String(t_def));
#endif

#ifdef _LM35_
  td.raw = td.raw / td.samples;
  td.samples = 1;
  td.temp = td.raw * C5V + (101325 - _pressure) * 3E-4;
#ifdef _DEBUG_
  Serial.println(" " + String(td.temp) + " " + String(td.raw) + " " + String(td.samples));
#endif
#endif

#ifdef _DEBUG_
  Serial.println("Sensors Process: " + String(millis() - b_t));
#endif
}

float SensorsProcess::readTemp(DeviceAddress therm)
{
  if (_sensors.isConnected(therm))
  {
    float temp = _sensors.getTempC(therm);
#ifdef _PRESS_
    if (_bmp280ready)
      temp += (101325 - _pressure) * 3E-4;
#endif
    return temp;
  }
  return 0.0;
}

#ifdef _LM35_

void SensorsProcess::updateLM35()
{
  td.raw += analogRead(td.pin);
  td.samples++;
}

#endif

void SensorsProcess::setPressure(float p)
{
  if (p > 93000 && p < 104000)
  {
    _pressure = p;
  }
}