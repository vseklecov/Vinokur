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
#ifndef _IR_
    setNextState();
#endif
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
      _delay = millis() + 20000; // TODO: 20 секунд
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
    {                                                    // 20 секунд
      if (abs(_sp->getThempKub() - temp_boiling) < 0.15) // 20 секунд не изменяется температура?
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

// Управление подачей охлаждения в холодильник дистиллятора
void DistillationProcess::setCooling(int procent)
{
  _cooling = procent;
}

// Управление нагревом куба
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
  { // EEPROM чистый
    _temp_cool_on = 75.0;
    _temp_def_cool_on = 50.0;
    _temp_distil_off = 98.0;     // 8%
    _temp_def_distil_off = 99.1; // 10%
    writeDataEEPROM();
  }
  else
  {
    int temp[4];
    EEPROM.get(2, temp); // Первый блок параметров
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
  { // EEPROM чистый
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
    {                                                   // 20 секунд
      if (abs(_sp->getThempKub() - temp_boiling) < 0.1) // 20 секунд не изменяется температура?
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
  if (EEPROM[0] == 255) // EEPROM чистый записать параметры прегонки браги
    DistillationProcess::readDataEEPROM();
  if (EEPROM[0] == 1)
  { // EEPROM с параметрами для перегонки браги, записать параметры для дробной перегонки
    _temp_cool_on = 75.0;
    _temp_def_cool_on = 50.0;
    _temp_distil_off = 98.0;     // 8%
    _temp_def_distil_off = 99.1; // 10%
    _equilibrate_time = 1200;    // 20 минут
    _temp_heart_off = 92.0;
    _temp_def_heart_off = 80.0;
    writeDataEEPROM();
  }
  else
  {
    int temp[7];
    EEPROM.get(10, temp); // Второй блок параметров
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
  if (EEPROM[0] == 255) // EEPROM чистый ОШИБКА!
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
