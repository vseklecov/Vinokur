#include <EEPROM.h>
#include <SPI.h>
#include <SD.h>

#include "Vinokur.h"
#include "Obmen.h"
#include "SensorsProcess.h"

extern SensorsProcess _sensors;

void initObmen()
{
  Serial.begin(BAUDRATE);
}

// Called when data is available.
void serialEvent()
{

  char command = (char)Serial.read();
  switch (command)
  {
  case HELP_HEADER:
    Serial.println("Список команд:");
    Serial.println("\tT ЧИСЛО\tУстановить время число");
    Serial.println("\tD\tУстановить время строка");
    Serial.println("\tP\tУстановить давление");
    Serial.println("\tS\tПослать файл-лог");
    Serial.println("\tR\tУдалить файл-лог");
    Serial.println("\tL\tПередать список файлов");
    Serial.println("\tG\tПоказать параметры");
    Serial.println("\tH\tПередать список команд");

    Serial.print('\t'); Serial.print(SET_VALUE_FLASH);
    Serial.println(" АДРЕС ЧИСЛО\tУстановить значение в флэш-памяти");
    break;
  case TIME_HEADER:
    {
      unsigned long pctime = Serial.parseInt();
      if (pctime > DEFAULT_TIME)
      {
        setTime(pctime);
      }
    }
    break;
  case DATE_HEADER:
    break;
  case PRES_HEADER:
    _sensors.setPressure(Serial.parseInt());
    break;
  case PARAMETERS:
    Serial.println("Версия:\t" + String(EEPROM[0]));
    if (EEPROM[0] > 0)
    {
      int temp[4];
      EEPROM.get(2, temp); // Первый блок параметров
      Serial.println("Первичная дистилляция:");
      Serial.println("(2) Охлаждение куб:\t" + String(temp[0] / 10));
      Serial.println("(4) Охлаждение деф:\t" + String(temp[1] / 10));
      Serial.println("(6) Окончание куб:\t" + String(temp[2] / 10));
      Serial.println("(8) Окончание деф:\t" + String(temp[3] / 10));
    }
    if (EEPROM[0] > 1)
    {
      int temp[7];
      EEPROM.get(10, temp); // Второй блок параметров
      Serial.println("Дробная дистилляция:");
      Serial.println("(10) Охлаждение куб:\t" + String(temp[0] / 10));
      Serial.println("(12) Охлаждение деф:\t" + String(temp[1] / 10));
      Serial.println("(14) Окончание куб:\t" + String(temp[2] / 10));
      Serial.println("(16) Окончание деф:\t" + String(temp[3] / 10));
      Serial.println("(18) Время работы на себя:\t" + String(temp[4]));
      Serial.println("(20) Хвосты куб:\t" + String(temp[5] / 10));
      Serial.println("(22) Хвосты деф:\t" + String(temp[6] / 10));
    }
    for (int i = 24; EEPROM[i] != 255; i++)
    {
      Serial.println(String(i) + ":\t" + String(EEPROM[i]));
    }
    break;
  case SET_VALUE_FLASH:
    {
      int addr = Serial.parseInt();
      int value = Serial.parseInt();
      if (addr>0 && value>0)
        EEPROM.put(addr, value);
    }
    break;
  case LIST_SD:
    {
      Sd2Card card;
      SdVolume volume;
      SdFile root;
      card.init(SPI_HALF_SPEED, SD_CS);
      volume.init(card);
      root.openRoot(volume);
      root.ls(LS_R | LS_DATE | LS_SIZE);
    }
    break;
#ifdef _LOG_
  case READ_FILE:
    {
      File dataFile = SD.open("distmash.csv", FILE_READ);
      if (dataFile)
      {
        while (dataFile.available())
        {
          Serial.write(dataFile.read());
        }
        Serial.write(END_OF_FILE);
        dataFile.close();
      }
    }
    break;
  case DEL_FILE:
    if (SD.exists("distmash.csv"))
    {
      File dataFile = SD.open("distmash.csv", O_WRITE | O_TRUNC);
      dataFile.close();
    }
    break;
#endif
  default:
    Serial.println((char)command);
  }
}

#ifndef _RTC_
// Time Sync provaider (Send time request to Serial)
time_t requestSync()
{
  Serial.write(TIME_REQUEST);
  return 0;
}
#endif
