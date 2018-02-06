#include <Arduino.h>
#include <UTFT.h>

extern UTFT myLCD;

#include "Button.h"

Button::Button(uint16_t x, uint16_t y, uint16_t w, uint16_t h, char *_label)
{
  pos_x = x;
  pos_y = y;
  width = w;
  height = h;
  label = _label;
  state = false;
}

Button::Button(uint16_t x, uint16_t y, char *_label)
{
  _font_text = myLCD.getFont();
  _fontXSize = myLCD.getFontXsize();
  _fontYSize = myLCD.getFontYsize();
  //Button(x, y, strlen(_label)+1)*_fontXSize, _fontYSize*2, _label);
  pos_x = x;
  pos_y = y;
  label = _label;
  width = (strlen(label) + 1) * _fontXSize;
  height = _fontYSize * 2;
  state = false;
  _color_text = VGA_AQUA;
  _color_text_inactive = VGA_GRAY;
  _color_background = VGA_BLUE;
  _color_border = VGA_WHITE;
  _color_hilite = VGA_RED;
}

Button::Button(uint16_t x, uint16_t y, String _label)
{
  _font_text = myLCD.getFont();
  _fontXSize = myLCD.getFontXsize();
  _fontYSize = myLCD.getFontYsize();
  pos_x = x;
  pos_y = y;
  label = (char *)malloc(_label.length() + 1);
  _label.toCharArray(label, _label.length() + 1);
  width = (strlen(label) + 1) * _fontXSize;
  height = _fontYSize * 2;
  state = false;
  _color_text = VGA_AQUA;
  _color_text_inactive = VGA_GRAY;
  _color_background = VGA_BLUE;
  _color_border = VGA_WHITE;
  _color_hilite = VGA_RED;
}

void Button::draw()
{

  uint8_t *_current_font = myLCD.getFont();
  ;
  word _current_color = myLCD.getColor();
  word _current_back = myLCD.getBackColor();

  uint16_t text_x = pos_x + width / 2 - (strlen(label) * _fontXSize) / 2;
  uint16_t text_y = pos_y + height / 2 - _fontYSize / 2;
  myLCD.setFont(_font_text);
  if (state)
  {
    myLCD.setColor(_color_background);
    myLCD.fillRoundRect(pos_x, pos_y, pos_x + width, pos_y + height);
    myLCD.setColor(_color_border);
    myLCD.drawRoundRect(pos_x, pos_y, pos_x + width, pos_y + height);
    myLCD.setColor(_color_text);
    myLCD.setBackColor(_color_background);
    myLCD.print(label, text_x, text_y);
  }
  else
  {
    myLCD.setColor(_current_back);
    myLCD.fillRoundRect(pos_x, pos_y, pos_x + width, pos_y + height);
    myLCD.setColor(_color_border);
    myLCD.drawRoundRect(pos_x, pos_y, pos_x + width, pos_y + height);
    myLCD.setColor(_color_text_inactive);
    myLCD.setBackColor(_current_back);
    myLCD.print(label, text_x, text_y);
  }
  myLCD.setFont(_current_font);
  myLCD.setColor(_current_color);
  myLCD.setBackColor(_current_back);
}

void Button::setTextFont(uint8_t *font)
{
  uint8_t *_current_font = myLCD.getFont();
  ;
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