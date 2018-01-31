#ifndef __VINOKUR_H__
#define __VINOKUR_H__

#include <Arduino.h>

#include <Time.h>
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
//#ifdef __AVR_ATmega2560__

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

//#endif
// ============== Конфигурация контактов в зависимости от используемой платы )))

#endif