#ifndef __BUTTON_H__
#define __BUTTON_H__

class Button
{
public:
  Button(uint16_t x, uint16_t y, uint16_t width, uint16_t height, char *_label);
  Button(uint16_t x, uint16_t y, char *_label);
  Button(uint16_t x, uint16_t y, String _label);

  void draw();
  boolean getState()
  {
    return state;
  };
  void setState(boolean st)
  {
    state = st;
  };
  void setTextFont(uint8_t *font);
  void setButtonColors(word atxt, word iatxt, word brd, word brdhi, word back);

private:
  uint16_t pos_x;
  uint16_t pos_y;
  uint16_t width;
  uint16_t height;
  char *label;
  boolean state;
  word _color_text, _color_text_inactive, _color_background, _color_border, _color_hilite;
  uint8_t *_font_text;
  byte _fontXSize;
  byte _fontYSize;
};

#endif