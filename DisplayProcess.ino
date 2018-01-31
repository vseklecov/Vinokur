#ifdef _TFT_
#include "DisplayProcess.h"

#ifdef _BUZ_
#include "Buzzer.h"
#endif

#define ENABLE  0x0000
#define DISABLE 0x0001
#define ALARM   0x0002
#define UNUSED  0x8000

extern uint8_t Tahoma[];

byte next_ch = 0;
char _time[] = {'0','0',':','0','0',':','0','0','\0'};

Button::Button(uint16_t x, uint16_t y, uint16_t w, uint16_t h, char *_label) {
  pos_x = x;
  pos_y = y;
  width = w;
  height = h;
  label = _label;
  state = false;
}

Button::Button(uint16_t x, uint16_t y, char *_label) {
  _font_text = myLCD.getFont();
  _fontXSize = myLCD.getFontXsize();
  _fontYSize = myLCD.getFontYsize();
  //Button(x, y, strlen(_label)+1)*_fontXSize, _fontYSize*2, _label);
  pos_x = x;
  pos_y = y;
  label = _label;
  width = (strlen(label)+1)*_fontXSize;
  height = _fontYSize*2;
  state = false;
  _color_text = VGA_AQUA;
  _color_text_inactive = VGA_GRAY;
  _color_background = VGA_BLUE;
  _color_border = VGA_WHITE;
  _color_hilite = VGA_RED;
}

Button::Button(uint16_t x, uint16_t y, String _label) {
  _font_text = myLCD.getFont();
  _fontXSize = myLCD.getFontXsize();
  _fontYSize = myLCD.getFontYsize();
  pos_x = x;
  pos_y = y;
  label = (char*)malloc(_label.length()+1);
  _label.toCharArray(label, _label.length()+1);
  width = (strlen(label)+1)*_fontXSize;
  height = _fontYSize*2;
  state = false;
  _color_text = VGA_AQUA;
  _color_text_inactive = VGA_GRAY;
  _color_background = VGA_BLUE;
  _color_border = VGA_WHITE;
  _color_hilite = VGA_RED;
}

void Button::draw() {
  
  uint8_t *_current_font = myLCD.getFont();;
  word  _current_color = myLCD.getColor();
  word  _current_back  = myLCD.getBackColor();

  uint16_t text_x = pos_x+width/2-(strlen(label)*_fontXSize)/2;
  uint16_t text_y = pos_y+height/2-_fontYSize/2;
  myLCD.setFont(_font_text);  
  if (state) {
    myLCD.setColor(_color_background);
    myLCD.fillRoundRect(pos_x,pos_y,pos_x+width,pos_y+height);
    myLCD.setColor(_color_border);
    myLCD.drawRoundRect(pos_x,pos_y,pos_x+width,pos_y+height);
    myLCD.setColor(_color_text);
    myLCD.setBackColor(_color_background);
    myLCD.print(label, text_x, text_y);
  } else {
    myLCD.setColor(_current_back);
    myLCD.fillRoundRect(pos_x,pos_y,pos_x+width,pos_y+height);
    myLCD.setColor(_color_border);
    myLCD.drawRoundRect(pos_x,pos_y,pos_x+width,pos_y+height);
    myLCD.setColor(_color_text_inactive);
    myLCD.setBackColor(_current_back);
    myLCD.print(label, text_x, text_y);
  }
  myLCD.setFont(_current_font);
  myLCD.setColor(_current_color);
  myLCD.setBackColor(_current_back);
}

void Button::setTextFont(uint8_t* font)
{
  uint8_t *_current_font = myLCD.getFont();;
  _font_text = font;
  myLCD.setFont(_font_text);  
  _fontXSize = myLCD.getFontXsize();
  _fontYSize = myLCD.getFontYsize();
  myLCD.setFont(_current_font);
}

void Button::setButtonColors(word atxt, word iatxt, word brd, word brdhi, word back)
{
  _color_text = atxt;
  _color_text_inactive = iatxt;
  _color_background = back;
  _color_border = brd;
  _color_hilite = brdhi;
}


Button *cool;
Button *heating;
Button *finish;
Button *alarm;


void DisplayProcess::setup()
{
  _myLCD->InitLCD();
  _myLCD->clrScr();
  _myLCD->setFont(Tahoma);
  _fontXSize=_myLCD->getFontXsize();
  _fontYSize=_myLCD->getFontYsize();

// Заставка
  _myLCD->setColor(VGA_AQUA);
  _myLCD->print(utf8rus("ВИНОКУР"), CENTER, _fontYSize*2);
  _myLCD->print(utf8rus("версия 0.15"), CENTER, _fontYSize*4);

  heating = new Button(_fontXSize/2, _fontYSize*5, utf8rus("НАГРЕВ"));
  cool    = new Button(_fontXSize*8, _fontYSize*5, utf8rus("ОХЛ"));
  finish  = new Button(_fontXSize*12.5, _fontYSize*5, utf8rus("КОНЕЦ"));
//  alarm = new Button(_fontXSize*5, _fontYSize*5, utf8rus("АВАРИЯ"));
//  alarm->setButtonColors(VGA_WHITE,VGA_GRAY,VGA_WHITE,VGA_RED,VGA_RED);
  
#ifdef _IR_
  _myLCD->print(utf8rus("Время:"), 0, _fontYSize*6);
  _myLCD->print(_time, _fontXSize*8, _fontYSize*6);
  _ir.setCallback(getTime);
#else
  curScreen = DISTIL_MASH;
  delay(1000);
#endif  
}


void DisplayProcess::changeScreen() {

  _myLCD->clrScr();
  switch (curScreen) {
    case MAIN_MENU: // Основное меню
      _myLCD->setColor(VGA_AQUA);
      _myLCD->print(utf8rus("Выбор режима работы:"), CENTER, 0);
      _myLCD->print(utf8rus("1. "+_distilMash.getHeader()), 0, _fontYSize);
      _myLCD->print(utf8rus("2. "+_refluxStill.getHeader()), 0, _fontYSize*2);
#ifdef _IR_
      _ir.setCallback(getMode);
#endif
      break;
    case DISTIL_MASH: // Первая перегонка браги
      _myLCD->setColor(VGA_AQUA);
      _myLCD->print(utf8rus(_distilMash.getHeader()), CENTER, 0);
      _myLCD->print(utf8rus("Куб"), _fontXSize, _fontYSize*2);
      _myLCD->print(utf8rus("Деф"), _fontXSize, _fontYSize*3);
#ifdef _LM35_
      _myLCD->print((char*)"TST", _fontXSize, _fontYSize*10);
#endif
      _myLCD->printChar('%', _fontXSize*16, _fontYSize*2);
      _myLCD->printChar('%', _fontXSize*16, _fontYSize*3);
#ifdef _PRESS_
      _myLCD->print(utf8rus("мм"), _fontXSize*18, _fontYSize*9);
#endif
      cool->draw();
      heating->draw();
      finish->draw();
      break;
    case FRACTION: // Дробная перегонка
      _myLCD->setColor(VGA_AQUA);
      _myLCD->print(utf8rus(_refluxStill.getHeader()), CENTER, 0);
      _myLCD->print(utf8rus("Куб"), _fontXSize, _fontYSize*2);
      _myLCD->print(utf8rus("Деф"), _fontXSize, _fontYSize*3);
      _myLCD->printChar('%', _fontXSize*16, _fontYSize*2);
      _myLCD->printChar('%', _fontXSize*16, _fontYSize*3);
#ifdef _PRESS_
      _myLCD->print(utf8rus("мм"), _fontXSize*18, _fontYSize*9);
#endif
      cool->draw();
      heating->draw();
      finish->draw();
      break;
    
    default:
      break;
  }
  prevScreen = curScreen;
}


void DisplayProcess::service()
{
#ifdef _DEBUG_
  unsigned long b_t = millis();
#endif

  if (curScreen != prevScreen) {
    changeScreen();
  }
  switch (curScreen) {
    case SPLASH:
      _myLCD->print(_time, _fontXSize*8, _fontYSize*6);
      if (next_ch !=0 ) {
        if (_time[next_ch-1]==':')
          showCursor(_fontXSize*(6+next_ch), _fontYSize*6, _myLCD->getBackColor());
        else
          showCursor(_fontXSize*(7+next_ch), _fontYSize*6, _myLCD->getBackColor());
      }
      showCursor(_fontXSize*(8+next_ch), _fontYSize*6, _myLCD->getColor());
      break;
    case MAIN_MENU:    // Основное меню
      break;
    case DISTIL_MASH:  // Первая перегонка браги
      _myLCD->setColor(VGA_AQUA);
      _myLCD->print(utf8rus(_distilMash.getStateString()), 0, _fontYSize);
#ifdef _DEBUG_
      Serial.println("Display Process: State string "+String(millis()-b_t));
      b_t = millis();
#endif
      _myLCD->print(formatTime(2,now()-_distilMash.getTimeBegin()), _fontXSize*7, _fontYSize);
      _myLCD->print(formatTime(3,0), CENTER, _fontYSize*9);
#ifdef _PRESS_
      _myLCD->printNumI(round(_sensors.getPressure()*7.5E-3), _fontXSize*15, _fontYSize*9, 3);
#endif

#ifdef _DEBUG_
      Serial.println("Display Process: format time string "+String(millis()-b_t));
      b_t = millis();
#endif
      _myLCD->printNumF(_sensors.getThempKub(), 1, _fontXSize*5, _fontYSize*2, '.', 5);
#ifdef _DEBUG_
      Serial.println("Display Process: Temp kub string "+String(millis()-b_t));
      b_t = millis();
#endif
      _myLCD->printNumF(_sensors.getThempDef(), 1, _fontXSize*5, _fontYSize*3, '.', 5);
#ifdef _DEBUG_
      Serial.println("Display Process: Temp def string "+String(millis()-b_t));
      b_t = millis();
#endif
#ifdef _LM35_
      _myLCD->printNumF(_sensors.getTD(), 1, _fontXSize*5, _fontYSize*10, '.', 5);
#endif
      _myLCD->printNumF(alcLiq(_sensors.getThempKub()), 1, _fontXSize*12, _fontYSize*2, '.', 4);
#ifdef _DEBUG_
      Serial.println("Display Process: Alc kub string "+String(millis()-b_t));
      b_t = millis();
#endif
      _myLCD->printNumF(alcSteam(_sensors.getThempDef()), 1, _fontXSize*12, _fontYSize*3, '.', 4);
#ifdef _DEBUG_
      Serial.println("Display Process: Alc def string "+String(millis()-b_t));
      b_t = millis();
#endif
      // Управление
      if (_distilMash.getHeating()!=heating->getState()) {
        heating->setState(_distilMash.getHeating());
        heating->draw();
      }      
      if (_distilMash.getCooling()!=cool->getState()) {
        cool->setState(_distilMash.getCooling());
        cool->draw();
#ifdef _BUZ_
        if (cool->getState())
          buzzer.playSong(&S);
#endif
      }      
      if ((_distilMash.getState()==STATE_END)!=finish->getState()) {
        finish->setState(_distilMash.getState()==STATE_END);
        finish->draw();
#ifdef _BUZ_
        if (finish->getState())
          buzzer.playSong(&S);
#endif
      }      
//      if (alarming) {
//        alarm->setState(alarm->getState());
//        alarm->draw();
//#ifdef _BUZ_
//        buzzer.playSong(Twinki);
//#endif
//      }      
      break;
    case FRACTION:
      _myLCD->setColor(VGA_AQUA);
      _myLCD->print(utf8rus(_refluxStill.getStateString()), 0, _fontYSize);
      _myLCD->print(formatTime(2,now()-_refluxStill.getTimeBegin()), _fontXSize*7, _fontYSize);
      _myLCD->print(formatTime(3,0), CENTER, _fontYSize*9);
#ifdef _PRESS_
      _myLCD->printNumI(round(_sensors.getPressure()*7.5E-3), _fontXSize*15, _fontYSize*9, 3);
#endif
      _myLCD->printNumF(_sensors.getThempKub(), 1, _fontXSize*5, _fontYSize*2, '.', 5);
      _myLCD->printNumF(_sensors.getThempDef(), 1, _fontXSize*5, _fontYSize*3, '.', 5);
      _myLCD->printNumF(alcLiq(_sensors.getThempKub()), 1, _fontXSize*12, _fontYSize*2, '.', 4);
      _myLCD->printNumF(alcSteam(_sensors.getThempDef()), 1, _fontXSize*12, _fontYSize*3, '.', 4);

      // Управление
      if (_refluxStill.getHeating()!=heating->getState()) {
        heating->setState(_refluxStill.getHeating());
        heating->draw();
      }      
      if (_refluxStill.getCooling()!=cool->getState()) {
        cool->setState(_refluxStill.getCooling());
        cool->draw();
//#ifdef _BUZ_
//        if (cool->getState())
//          buzzer.playSong(&S);
//#endif
      }      
      if ((_refluxStill.getState()==STATE_END)!=finish->getState()) {
        finish->setState(_refluxStill.getState()==STATE_END);
        finish->draw();
//#ifdef _BUZ_
//        if (finish->getState())
//          buzzer.playSong(&S);
//#endif
      }      
      break;
    
    default:
      break;
  }

#ifdef _DEBUG_
  Serial.println("Display Process: "+String(millis()-b_t));
#endif
}

/* Перекодировка русских символов из UTF-8 в Windows-1251 со смещением 0x40(Ё) как в Tahoma*/
String DisplayProcess::utf8rus(String source)
{
  int i = 0;
  int k = source.length();
  String target;
  unsigned char n;
  char m[2] = { '0', '\0' };

  while (i < k) {
    n = source[i++];

    if (n >= 0xC0) {
      switch (n) {
        case 0xD0: {
          n = source[i++];
          if (n == 0x81) { n = 0x40; break; }       //0x40 Ё
          if (n >= 0x90 && n <= 0xBF) n = n - 0x4E; //0x42 А-Я, а-п
          break;
        }
        case 0xD1: {
          n = source[i++];
          if (n == 0x91) { n = 0x41; break; }       //0x41 ё
          if (n >= 0x80 && n <= 0x8F) n = n - 0x0E; //0x72 р-я
          break;
        }
      }
    }
    m[0] = n; target = target + String(m);
  }
  return target;
}


void DisplayProcess::showCursor(int x, int y, word color)
{
  word fcolor = _myLCD->getColor();
  _myLCD->setColor(color);
  _myLCD->fillRect(x,y+_fontYSize-1,x+_fontXSize,y+_fontYSize);
  _myLCD->setColor(fcolor);
}


#ifdef _IR_
void getTime(char ch)
{
  int Hour, Min, Sec;
  switch (ch)
  {
    case '\0':
      return;

    case '\n':
      break;

    case '\b':
    {
      if (next_ch==0)
        return;
      next_ch--;
      if (_time[next_ch]==':')    
        next_ch--;
      _time[next_ch]='0';
      return;
    }
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    {
      _time[next_ch] = ch;
      next_ch++;
      if (next_ch>7)
        break;
      if (_time[next_ch]==':')
        next_ch++;
      return;
    }
    default:
      return;    
  }
  if (sscanf(_time, "%d:%d:%d", &Hour, &Min, &Sec) == 3)
    setTime(Hour, Min, Sec, day(), month(), year());
  _ir.setCallback(NULL);
  _display.setCurrentScreen(MAIN_MENU);
}

void getMode(char ch)
{
  switch (ch)
  {
    case '1':
      _ir.setCallback(controlProcess);
      _display.setCurrentScreen(DISTIL_MASH);
      _distilMash.setState(STATE_STOPS);
      _distilMash.enable();
      break;
    case '2':
      _ir.setCallback(controlProcess);
      _display.setCurrentScreen(FRACTION);
      _refluxStill.setState(STATE_STOPS);
      _refluxStill.enable();
      break;
    default:
      break;  
  }
}

void getStartStop(char ch)
{
  switch (ch)
  {
    case '1':
      _ir.setCallback(NULL);
      _display.setCurrentScreen(DISTIL_MASH);
      _distilMash.enable();
      break;
    case '2':
      break;
    default:
      break;  
  }
}

#endif

#endif

