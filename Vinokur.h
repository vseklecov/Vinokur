#ifndef __VINOKUR_H__
#define __VINOKUR_H__

#include <Arduino.h>

#include <Time.h>
#include <EEPROM.h>

// ============== ��������� ������� ������
// �������� ���������� ��������� ����� COM-����
//#define _DEBUG_
// ������ ���� �� SD �����
#define _LOG_
// ���������� ������ ����������� LM35
//#define _LM35_
// ������������ �������� �������
#define _BUZ_
// ������������ TFT ��-������� 3.2 ����� Ultra HD 320x480 ILI9481 ����������
#define _TFT_
#ifdef _TFT_
// ������������ ���������� 16 ������ 4x4 ������� (��� ������ �� ������������)
//#define _KEYPAD_
// ������������ ������������ ��������
#define _IR_
#endif
// ������������ ���� ��������� ������� I2C RTC DS1307(����) AT24C32(����-������)
//#define _RTC_
// ������������ ������ �������� I2C/SPI BMP280
#define _PRESS_
// ============== ��������� ������� ������ )))

// ============== ������������ ��������� � ����������� �� ������������ �����

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
// SD ����� �� ����� ��-�������
const byte SD_CS = 53;
#endif

#ifdef _KEYPAD_
const byte KEY_ROW[ROWS] = {5, 4, 3, 2};
const byte KEY_COL[COLS] = {9, 8, 7, 6};
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
// ============== ������������ ��������� � ����������� �� ������������ ����� )))

const long DEFAULT_TIME = 1483228800; //2017-01-01 00:00:00

#endif