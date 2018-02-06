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

extern uint8_t Tahoma[];

#include "Button.h"

Button *cool;
Button *heating;
Button *finish;
Button *alarm;

byte next_ch = 0;
char _time[] = {'0', '0', ':', '0', '0', ':', '0', '0', '\0'};

void DisplayProcess::setup()
{
  _myLCD->InitLCD();
  _myLCD->clrScr();
  _myLCD->setFont(Tahoma);
  _fontXSize = _myLCD->getFontXsize();
  _fontYSize = _myLCD->getFontYsize();

  // Заставка
  _myLCD->setColor(VGA_AQUA);
  _myLCD->print(utf8rus("ВИНОКУР"), CENTER, _fontYSize * 2);
  _myLCD->print(utf8rus("версия 0.15"), CENTER, _fontYSize * 4);

  heating = new Button(_fontXSize / 2, _fontYSize * 5, utf8rus("НАГРЕВ"));
  cool = new Button(_fontXSize * 8, _fontYSize * 5, utf8rus("ОХЛ"));
  finish = new Button(_fontXSize * 12.5, _fontYSize * 5, utf8rus("КОНЕЦ"));

#ifdef _IR_
  //Input *param1 = new Input(_myLCD, 0, _fontYSize, "1234567890:", "000", '.', '0');
  //param1->draw();
  _myLCD->print(utf8rus("Время:"), 0, _fontYSize * 6);
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
  case MAIN_MENU: // Основное меню
    _myLCD->setColor(VGA_AQUA);
    _myLCD->print(utf8rus("Выбор режима работы:"), CENTER, 0);
    _myLCD->print(utf8rus("0. Параметры"), 0, _fontYSize);
    _myLCD->print(utf8rus("1. " + _distilMash.getHeader()), 0, _fontYSize * 2);
    _myLCD->print(utf8rus("2. " + _refluxStill.getHeader()), 0, _fontYSize * 3);
#ifdef _IR_
    _ir.setCallback(getMode);
#endif
    break;
  case PARAMETERS: // Параметры
  {
    Input *param1 = new Input(_myLCD, 0, _fontYSize, "1234567890:", "000", '.', '0');
    param1->draw();
  }
    break;
  case DISTIL_MASH: // Первая перегонка браги
    _myLCD->setColor(VGA_AQUA);
    _myLCD->print(utf8rus(_distilMash.getHeader()), CENTER, 0);
    _myLCD->print(utf8rus("Куб"), _fontXSize, _fontYSize * 2);
    _myLCD->print(utf8rus("Деф"), _fontXSize, _fontYSize * 3);
#ifdef _LM35_
    _myLCD->print((char *)"TST", _fontXSize, _fontYSize * 10);
#endif
    _myLCD->printChar('%', _fontXSize * 16, _fontYSize * 2);
    _myLCD->printChar('%', _fontXSize * 16, _fontYSize * 3);
#ifdef _PRESS_
    _myLCD->print(utf8rus("мм"), _fontXSize * 18, _fontYSize * 9);
#endif
    cool->draw();
    heating->draw();
    finish->draw();
    break;
  case FRACTION: // Дробная перегонка
    _myLCD->setColor(VGA_AQUA);
    _myLCD->print(utf8rus(_refluxStill.getHeader()), CENTER, 0);
    _myLCD->print(utf8rus("Куб"), _fontXSize, _fontYSize * 2);
    _myLCD->print(utf8rus("Деф"), _fontXSize, _fontYSize * 3);
    _myLCD->printChar('%', _fontXSize * 16, _fontYSize * 2);
    _myLCD->printChar('%', _fontXSize * 16, _fontYSize * 3);
#ifdef _PRESS_
    _myLCD->print(utf8rus("мм"), _fontXSize * 18, _fontYSize * 9);
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
    _myLCD->print(_time, _fontXSize * 8, _fontYSize * 6);
    if (next_ch != 0)
    {
      if (_time[next_ch - 1] == ':')
        showCursor(_fontXSize * (6 + next_ch), _fontYSize * 6, _myLCD->getBackColor());
      else
        showCursor(_fontXSize * (7 + next_ch), _fontYSize * 6, _myLCD->getBackColor());
    }
    showCursor(_fontXSize * (8 + next_ch), _fontYSize * 6, _myLCD->getColor());
    break;
  case MAIN_MENU: // Основное меню
    break;
  case DISTIL_MASH: // Первая перегонка браги
    _myLCD->setColor(VGA_AQUA);
    _myLCD->print(utf8rus(_distilMash.getStateString()), 0, _fontYSize);
#ifdef _DEBUG_
    Serial.println("Display Process: State string " + String(millis() - b_t));
    b_t = millis();
#endif
    _myLCD->print(formatTime(2, now() - _distilMash.getTimeBegin()), _fontXSize * 7, _fontYSize);
    _myLCD->print(formatTime(3, 0), CENTER, _fontYSize * 9);
#ifdef _PRESS_
    _myLCD->printNumI(round(_sensors.getPressure() * 7.5E-3), _fontXSize * 15, _fontYSize * 9, 3);
#endif

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
    // Управление
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
    _myLCD->printNumF(_sensors.getThempKub(), 1, _fontXSize * 5, _fontYSize * 2, '.', 5);
    _myLCD->printNumF(_sensors.getThempDef(), 1, _fontXSize * 5, _fontYSize * 3, '.', 5);
    _myLCD->printNumF(alcLiq(_sensors.getThempKub()), 1, _fontXSize * 12, _fontYSize * 2, '.', 4);
    _myLCD->printNumF(alcSteam(_sensors.getThempDef()), 1, _fontXSize * 12, _fontYSize * 3, '.', 4);

    // Управление
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

/* Перекодировка русских символов из UTF-8 в Windows-1251 со смещением 0x40(Ё) как в Tahoma*/
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
        } //0x40 Ё
        if (n >= 0x90 && n <= 0xBF)
          n = n - 0x4E; //0x42 А-Я, а-п
        break;
      }
      case 0xD1:
      {
        n = source[i++];
        if (n == 0x91)
        {
          n = 0x41;
          break;
        } //0x41 ё
        if (n >= 0x80 && n <= 0x8F)
          n = n - 0x0E; //0x72 р-я
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

#endif