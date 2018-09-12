#include "Vinokur.h"
#include "Obmen.h"

// ============== Инициализация глобальных переменных

#include <ProcessScheduler.h>
Scheduler sched;

#include "SensorsProcess.h"
OneWire ds(ONE_WIRE_BUS);
SensorsProcess _sensors(sched, HIGH_PRIORITY, 1000, &ds);

#include "DistillationProcess.h"
DistillMashProcess _distilMash(sched, HIGH_PRIORITY, 500, &_sensors);
FractioningProcess _refluxStill(sched, HIGH_PRIORITY, 500, &_sensors);

#include "Termostat.h"
TermostatProcess termostat(sched, HIGH_PRIORITY, 500, _sensors);

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
IRProcess _ir(sched, HIGH_PRIORITY, 100, IR_PIN);
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

unsigned long cMillis; // Текущее время

// Состояние процесса
//enum distillation {MAIN, FIRST, STEAM, FIRST_G, FRACTIONAL} state = MAIN;

// ============== Инициализация глобальных переменных )))

void setup()
{

  initObmen();

  _sensors.add(true);
#ifndef _IR_
  _distilMash.add(true);
#else
  _distilMash.add(false);
  _refluxStill.add(false);
  termostat.add(false);
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
  //setTime(DEFAULT_TIME);
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
  delay(20);
}
