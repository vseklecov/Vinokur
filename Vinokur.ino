#include <Arduino.h>

#include <Time.h>
#include <TimeLib.h>
#include <EEPROM.h>

// ============== Настройка состава скетча
// Посылать отладочные сообщения через COM-порт
//#define _DEBUG_
// Запись лога на SD карту
#define _LOG_
// Опрашивать датчик температуры LM35
//#define _LM35_
// Использовать звуковые сигналы
#define _BUZ_
// Использовать TFT ЖК-дисплей 3.2 дюйма Ultra HD 320x480 ILI9481 контроллер
#define _TFT_
#ifdef _TFT_
  // Использовать клавиатуру 16 клавиш 4x4 матрица (без экрана на использовать)
  //#define _KEYPAD_
  // Использовать инфракрасный приемник
  #define _IR_
#endif
// Использовать часы реального времени I2C RTC DS1307(часы) AT24C32(флэш-память) 
//#define _RTC_
// Использовать датчик давления I2C/SPI BMP280
#define _PRESS_
// ============== Настройка состава скетча )))

// ============== Конфигурация контактов в зависимости от используемой платы

// Arduino Mega 2560
#ifdef __AVR_ATmega2560__

const byte ONE_WIRE_BUS = 10;

#ifdef _TFT_
const byte TFT_RS = 38;
const byte TFT_WR = 39;
const byte TFT_CS = 40;
const byte TFT_RST = 41;
#endif

#ifdef _LOG_
// SD карта на плате ЖК-дисплея
const byte SD_CS = 53;
#endif

#ifdef _KEYPAD_
const byte KEY_ROW[ROWS] = {5,4,3,2};
const byte KEY_COL[COLS] = {9,8,7,6};
#endif

#ifdef _BUZ_
const byte BUZZER = 11;
#endif

#ifdef _IR_
const byte IR_PIN = 12;
#endif

#ifdef _LM35_
const byte LM35 = A0;
#endif

#endif
// ============== Конфигурация контактов в зависимости от используемой платы )))

#include <ProcessScheduler.h>

#include "SensorsProcess.h"
#include "DistillationProcess.h"

#ifdef _TFT_
#include "DisplayProcess.h"
#endif

#ifdef _LOG_
#include <SD.h>
#include "LoggerProcess.h"
#endif

#ifdef _KEYPAD_
#include "KeypadProcess.h"
#endif

#ifdef _IR_
#include "IRProcess.h"
#endif

#ifdef _RTC_
#include <Wire.h>
#include <DS1307RTC.h>
#endif

#include <SPI.h>

#include "Buzzer.h"

// ============== Инициализация глобальных переменных
Scheduler sched;

OneWire ds(ONE_WIRE_BUS);
SensorsProcess _sensors(sched, HIGH_PRIORITY, 1000, &ds);
DistillMashProcess _distilMash(sched, HIGH_PRIORITY, 500, &_sensors);
FractioningProcess _refluxStill(sched, HIGH_PRIORITY, 500, &_sensors);

#ifdef _TFT_
UTFT myLCD(CTE32HR, TFT_RS, TFT_WR, TFT_CS, TFT_RST);
DisplayProcess _display(sched, MEDIUM_PRIORITY, 1000, &myLCD);
#endif

#ifdef _LOG_
LoggerProcess _logger(sched, MEDIUM_PRIORITY, 10000);
#endif

#ifdef _KEYPAD_
KeypadProcess _keypad(sched, HIGH_PRIORITY, 500);
#endif

#ifdef _IR_
IRProcess _ir(sched, HIGH_PRIORITY, 500, IR_PIN);
#endif

#ifdef _BUZ_
Buzzer buzzer(sched, HIGH_PRIORITY, 50, BUZZER);
#endif

const long DEFAULT_TIME = 1483228800; //2017-01-01 00:00:00

unsigned long cMillis; // Текущее время

// Включение охлаждения
//boolean cooling  =false;
// Окончание процесса дистилляции
//boolean finishing=false;
// Авария - СТОП
//boolean alarming =false;

// Состояние процесса
enum distillation {MAIN, FIRST, STEAM, FIRST_G, FRACTIONAL} state = MAIN;

// ============== Инициализация глобальных переменных )))

void setup() {

  initObmen();

//  for(int i=0; i<25;i++) {
//    byte b = EEPROM.read(i);
//    Serial.print(i, DEC);
//    Serial.print(":\t");
//    Serial.println(b, DEC);
//  }
    
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
  if (SD.begin(SD_CS)) {
    _logger.add(true);
  }
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
  if (timeStatus()!=timeSet)
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


void loop() {
  
#ifdef _LM35_
  _sensors.updateLM35();
#endif

  sched.run();
}

