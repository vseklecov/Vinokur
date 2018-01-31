#include <EEPROM.h>
#include <SD.h>

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


void initObmen() {
  Serial.begin(BAUDRATE);
}


// Called when data is available.
void serialEvent() {
  
  char command=(char)Serial.read();
  switch (command) {
    case TIME_HEADER:
      {
        unsigned long pctime = Serial.parseInt();
        if (pctime > DEFAULT_TIME) {
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
      Serial.println("Версия:\t"+String(EEPROM[0]));
      //Serial.println("1:\t"+String(EEPROM[1]));
      if (EEPROM[0]>0) {
        int temp[4];
        EEPROM.get(2, temp); // Первый блок параметров
        Serial.println("Первичная дистилляция:");
        Serial.println("Охлаждение куб:\t"+String(temp[0]/10));
        Serial.println("Охлаждение деф:\t"+String(temp[1]/10));
        Serial.println("Окончание куб:\t"+String(temp[2]/10));
        Serial.println("Окончание деф:\t"+String(temp[3]/10));
      }
      if (EEPROM[0]>1) {
        int temp[7];
        EEPROM.get(10, temp); // Второй блок параметров
        Serial.println("Дробная дистилляция:");
        Serial.println("Охлаждение куб:\t"+String(temp[0]/10));
        Serial.println("Охлаждение деф:\t"+String(temp[1]/10));
        Serial.println("Окончание куб:\t"+String(temp[2]/10));
        Serial.println("Окончание деф:\t"+String(temp[3]/10));
        Serial.println("Время работы на себя:\t"+String(temp[4]));
        Serial.println("Хвосты куб:\t"+String(temp[5]/10));
        Serial.println("Хвосты деф:\t"+String(temp[6]/10));
      }
      for (int i=24;EEPROM[i]!=255;i++) {
        Serial.println(String(i)+":\t"+String(EEPROM[i]));
      }
      break;
    case LIST_SD:
      {
        File root = SD.open("/");
        Serial.println(root.name());
        File entry = root.openNextFile();
        Serial.println(entry.name());
        while (entry) {
          Serial.print(entry.name());
          if (entry.isDirectory())
            Serial.println("/");
          else {
            Serial.print("\t\t");
            Serial.println(entry.size(), DEC);
          }
          entry.close();
          entry = root.openNextFile();
        }
        Serial.println("Done...");
      }
      break;
#ifdef _LOG_
    case READ_FILE:
      {
        File dataFile = SD.open("distmash.csv", FILE_READ);
        if (dataFile) {
          while (dataFile.available()) {
            Serial.write(dataFile.read());
          }
          Serial.write(END_OF_FILE);
          dataFile.close();
        }
      }
      break;
    case DEL_FILE:
      if (SD.exists("distmash.csv")) {
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
time_t requestSync() {
  //Serial.write(TIME_REQUEST);
  return 0;
}
#endif

