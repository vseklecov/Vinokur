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
    Serial.println("������ ������:");
    Serial.println("\tT �����\t���������� ����� �����");
    Serial.println("\tD\t���������� ����� ������");
    Serial.println("\tP\t���������� ��������");
    Serial.println("\tS\t������� ����-���");
    Serial.println("\tR\t������� ����-���");
    Serial.println("\tL\t�������� ������ ������");
    Serial.println("\tG\t�������� ���������");
    Serial.println("\tH\t�������� ������ ������");

    Serial.print('\t'); Serial.print(SET_VALUE_FLASH);
    Serial.println(" ����� �����\t���������� �������� � ����-������");
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
    Serial.println("������:\t" + String(EEPROM[0]));
    if (EEPROM[0] > 0)
    {
      int temp[4];
      EEPROM.get(2, temp); // ������ ���� ����������
      Serial.println("��������� �����������:");
      Serial.println("(2) ���������� ���:\t" + String(temp[0] / 10));
      Serial.println("(4) ���������� ���:\t" + String(temp[1] / 10));
      Serial.println("(6) ��������� ���:\t" + String(temp[2] / 10));
      Serial.println("(8) ��������� ���:\t" + String(temp[3] / 10));
    }
    if (EEPROM[0] > 1)
    {
      int temp[7];
      EEPROM.get(10, temp); // ������ ���� ����������
      Serial.println("������� �����������:");
      Serial.println("(10) ���������� ���:\t" + String(temp[0] / 10));
      Serial.println("(12) ���������� ���:\t" + String(temp[1] / 10));
      Serial.println("(14) ��������� ���:\t" + String(temp[2] / 10));
      Serial.println("(16) ��������� ���:\t" + String(temp[3] / 10));
      Serial.println("(18) ����� ������ �� ����:\t" + String(temp[4]));
      Serial.println("(20) ������ ���:\t" + String(temp[5] / 10));
      Serial.println("(22) ������ ���:\t" + String(temp[6] / 10));
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
