#include <Arduino.h>
#include <avr/pgmspace.h>
#include <Time.h>

#include "Functions.h"

const float steam[218] PROGMEM = {
                96.3, 95.2, 94.7, 94.2, 93.8, 93.1, 92.7, 92.2, // 78.2 - 78.9
    91.9, 91.6, 91.2, 90.8, 90.5, 90.1, 89.6, 89.1, 88.4, 88.1, // 79.0 - 79.9
    87.8, 87.4, 87.2, 87.0, 86.8, 86.3, 86.1, 85.8, 85.6, 85.4, // 80.0 - 80.9
    85.1, 84.8, 84.7, 84.3, 84.0, 83.8, 83.6, 83.4, 83.3, 83.0, // 81.0 - 81.9
    82.8, 82.7, 82.5, 82.4, 82.2, 81.8, 81.7, 81.6, 81.4, 81.2, // 82.0 - 82.9
    81.1, 81.0, 80.8, 80.7, 80.4, 80.3, 80.1, 80.0, 79.6, 79.5, // 83.0 - 83.9
    79.4, 79.2, 79.1, 78.8, 78.6, 78.5, 78.3, 78.1, 77.7, 77.4, // 84.0 - 84.9
    77.2, 77.0, 76.8, 76.6, 76.3, 76.1, 75.9, 75.7, 75.5, 75.3, // 85.0 - 85.9
    75.0, 74.8, 74.5, 74.2, 73.9, 73.6, 73.4, 73.2, 72.9, 72.7, // 86.0 - 86.9
    72.4, 72.3, 72.1, 71.9, 71.8, 71.4, 71.1, 70.8, 70.3, 69.8, // 87.0 - 87.9
    69.6, 69.3, 69.0, 68.8, 68.6, 68.5, 68.3, 68.2, 68.0, 67.9, // 88.0 - 88.9
    67.7, 67.2, 66.6, 66.3, 66.1, 65.8, 65.5, 65.2, 65.1, 64.9, // 89.0 - 89.9
    64.7, 64.5, 64.3, 64.1, 63.9, 63.7, 63.1, 62.6, 62.0, 61.6, // 90.0 - 90.9
    61.2, 60.9, 60.5, 60.1, 59.7, 59.4, 59.0, 58.6, 58.3, 57.9, // 91.0 - 91.9
    57.5, 57.2, 56.8, 56.5, 56.1, 55.8, 55.4, 55.0, 54.6, 54.2, // 92.0 - 92.9
    53.8, 53.3, 52.9, 52.5, 52.1, 51.6, 51.1, 50.7, 50.2, 49.7, // 93.0 - 93.9
    49.3, 48.8, 48.3, 47.7, 47.2, 46.6, 46.1, 45.5, 45.0, 44.4, // 94.0 - 94.9
    43.9, 43.4, 42.9, 42.4, 41.9, 41.5, 41.0, 40.5, 40.0, 39.2, // 95.0 - 95.9
    38.4, 37.5, 36.7, 35.9, 35.1, 34.3, 33.4, 32.6, 31.7, 30.8, // 96.0 - 96.9
    29.9, 29.1, 28.2, 27.3, 26.4, 25.5, 24.6, 23.7, 22.7, 21.7, // 97.0 - 97.9
    20.8, 19.8, 18.8, 17.8, 16.8, 15.8, 14.9, 13.9, 13.0, 12.3, // 98.0 - 98.9
    11.2, 10.0, 8.9, 7.6, 6.1, 4.8, 3.3, 2.5,  1.6,  0.2        // 99.0 - 99.9
};

const float liq[] PROGMEM = {
                96.1, 94.7, 94.0, 93.3, 92.5, 91.4, 90.7, 89.9, // 78.2 - 78.9
    89.2, 88.7, 87.9, 87.1, 86.3, 85.5, 84.3, 83.0, 80.9, 80.0, // 79.0 - 79.9
    78.7, 77.8, 77.0, 76.1, 75.2, 73.3, 72.4, 71.0, 70.1, 69.2, // 80.0 - 80.9
    67.7, 66.8, 65.8, 63.9, 62.9, 61.9, 60.9, 59.9, 58.9, 57.9, // 81.0 - 81.9
    56.9, 56.2, 55.5, 54.8, 53.2, 51.7, 51.0, 50.2, 49.4, 48.8, // 82.0 - 82.9
    48.1, 47.4, 46.9, 46.3, 45.2, 44.2, 43.6, 43.0, 41.4, 40.6, // 83.0 - 83.9
    40.3, 40.0, 39.7, 38.5, 38.0, 37.4, 36.8, 36.3, 35.1, 34.5, // 84.0 - 84.9
    34.0, 33.4, 32.8, 32.2, 31.6, 31.2, 30.9, 30.5, 30.0, 29.5, // 85.0 - 85.9
    29.1, 28.6, 28.1, 27.5, 26.9, 26.5, 26.1, 25.7, 25.3, 24.9, // 86.0 - 86.9
    24.5, 24.2, 23.9, 23.6, 23.3, 22.9, 22.5, 22.1, 21.5, 20.9, // 87.0 - 87.9
    20.6, 20.4, 20.1, 19.8, 19.6, 19.4, 19.2, 19.1, 18.9, 18.7, // 88.0 - 88.9
    18.5, 17.9, 17.3, 17.1, 16.8, 16.6, 16.4, 16.1, 16.0, 15.8, // 89.0 - 89.9
    15.7, 15.5, 15.4, 15.2, 15.0, 14.9, 14.5, 14.1, 13.7, 13.4, // 90.0 - 90.9
    13.2, 12.9, 12.7, 12.5, 12.3, 12.1, 11.8, 11.6, 11.4, 11.2, // 91.0 - 91.9
    11.0, 10.9, 10.7, 10.5, 10.3, 10.2, 10.0,  9.8,  9.7,  9.5, // 92.0 - 92.9
     9.3,  9.2,  9.0,  8.8,  8.7,  8.5,  8.4,  8.2,  8.1,  7.9, // 93.0 - 93.9
     7.8,  7.6,  7.4,  7.3,  7.1,  7.0,  6.8,  6.7,  6.5,  6.4, // 94.0 - 94.9
     6.2,  6.1,  5.9,  5.8,  5.6,  5.5,  5.3,  5.2,  5.0,  4.9, // 95.0 - 95.9
     4.7,  4.6,  4.5,  4.3,  4.2,  4.0,  3.9,  3.7,  3.6,  3.5, // 96.0 - 96.9
     3.4,  3.2,  3.1,  3.0,  2.9,  2.7,  2.6,  2.5,  2.4,  2.3, // 97.0 - 97.9
     2.1,  2.0,  1.9,  1.8,  1.7,  1.6,  1.5,  1.3,  1.2,  1.2, // 98.0 - 98.9
     1.0,  0.9,  0.8,  0.6,  0.5,  0.4,  0.3,  0.2,  0.1,  0.0  // 99.0 - 99.9
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

// f - ������ (1-'YYYY-MM-DD HH:MM:SS', 2-'HH:MM:SS', 3-'HH:MM')
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

/* ������������� ������� �������� �� UTF-8 � Windows-1251 �� ��������� 0x40(�) ��� � Tahoma*/
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
        } //0x40 �
        if (n >= 0x90 && n <= 0xBF)
          n = n - 0x4E; //0x42 �-�, �-�
        break;
      }
      case 0xD1:
      {
        n = source[i++];
        if (n == 0x91)
        {
          n = 0x41;
          break;
        } //0x41 �
        if (n >= 0x80 && n <= 0x8F)
          n = n - 0x0E; //0x72 �-�
        break;
      }
      }
    }
    buf[j++] = n;
  }
  buf[j] = '\0';
  return buf;
}
