#ifndef __FUNCTIONS_H__
#define __FUNCTIONS_H__

#include <Arduino.h>
#include <Time.h>

float alcSteam(float);
float alcLiq(float);
String formatTime(byte, time_t);
char *utf8rusch(const char *, char *);
#endif