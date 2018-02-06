#ifndef __DISTILL_MASH__
#define __DISTILL_MASH__

#include <EEPROM.h>
#include <TimeLib.h>
#include <ProcessScheduler.h>

#include "SensorsProcess.h"
#include "Keys.h"

const char PROCESS_START = KEY_PLAY;
const char PROCESS_CANCEL = KEY_ON;
const char PROCESS_PREV_STATE = KEY_PREV;
const char PROCESS_NEXT_STATE = KEY_NEXT;
const char PROCESS_SETTINGS = KEY_EQ;

typedef enum washes { WASH_SUGAR,
                      WASH_GRAIN,
                      WASH_FRUIT,
                      WASH_SPIRIT } washes;
typedef enum states { STATE_STOPS,
                      STATE_HEAT,
                      STATE_DISTIL,
                      STATE_EQUIL,
                      STATE_HEAD,
                      STATE_HEART,
                      STATE_TAILS,
                      STATE_END,
                      STATE_CANCEL } states;

class DistillationProcess : public Process
{
public:
  DistillationProcess(Scheduler &manager, ProcPriority pr, unsigned int period, SensorsProcess *sp)
      : Process(manager, pr, period)
  {
    _sp = sp;
    boiling_kub = 0;
    steam_def = 0;
    _delay = 0;
    _state = STATE_STOPS;
    readDataEEPROM();
  }

  void setup(void);
  void service(void);

  states getState() { return _state; }
  boolean getCooling() { return _cooling > 0; }
  boolean getHeating() { return _heating > 0; }
  time_t getTimeBegin() { return time_begin; }

  void setHeating(int);
  void setCooling(int);

  String getStateString(void);
  void setState(states state);
  void setNextState(void);
  void setPrevState(void);

  void readDataEEPROM(void);
  void writeDataEEPROM(void);

  virtual String getHeader() { return ""; }

  float boiling_kub;
  float steam_def;

  float temp_boiling;
  //  private:
  SensorsProcess *_sp;
  states _state;
  float _temp_cool_on;        // Температура куба для включения охлаждения
  float _temp_def_cool_on;    // Температура перед холодильником для включения охлаждения
  float _temp_distil_off;     // Температура куба окончания дистилляции
  float _temp_def_distil_off; // Температура перед холодильником окончания дистилляции

  int _cooling;
  int _heating;
  time_t time_begin;
  unsigned long time_boiling;
  unsigned long _delay;
};

class DistillMashProcess : public DistillationProcess
{
public:
  DistillMashProcess(Scheduler &manager, ProcPriority pr, unsigned int period, SensorsProcess *sp)
      : DistillationProcess(manager, pr, period, sp) {}

  String getHeader() { return "Дистилляция браги"; }
};

class FractioningProcess : public DistillationProcess
{
public:
  FractioningProcess(Scheduler &manager, ProcPriority pr, unsigned int period, SensorsProcess *sp)
      : DistillationProcess(manager, pr, period, sp)
  {
    readDataEEPROM();
  }

  void service(void);

  void setNextState(void);
  void setPrevState(void);

  void readDataEEPROM(void);
  void writeDataEEPROM(void);

  String getHeader() { return "Дробная перегонка"; }

  //  private:
  int _equilibrate_time;     // Время работы на себя колонны в секундах
  float _temp_heart_off;     // Температура куба окончания отбора тела
  float _temp_def_heart_off; // Температура перед холодильником окончания отбора тела
};

#endif
