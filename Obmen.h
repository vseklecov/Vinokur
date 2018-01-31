#ifndef __OBMEN_H__
#define __OBMEN_H__

const long BAUDRATE    = 115200;

const char TIME_HEADER = 'T'; // Set time
const char DATE_HEADER = 'D'; // Set date text
const char PRES_HEADER = 'P'; // Set pressure
const char READ_FILE   = 'S'; // Send log file
const char DEL_FILE    = 'R'; // Remove log file
const char PARAMETERS  = 'G'; // Передать все параметры
const char LIST_SD     = 'L'; // Передать список файлов

const byte END_OF_FILE = 12;
const byte TIME_REQUEST= 7;

void initObmen();
#ifndef _RTC_
time_t requestSync();
#endif

#endif 