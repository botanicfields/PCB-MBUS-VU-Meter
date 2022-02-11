// Copyright 2022 BotanicFields, Inc.
// BF-037 VU meters on M5Stack

#include <M5GFX.h>
#include "BF_M5StackVuMeter.h"

static M5GFX lcd;
static M5Canvas canvas(&lcd);    // final image of VU meter
static M5Canvas panel(&canvas);  // panel image of VU meter
static M5Canvas mask(&panel);    // mask to make VU-meter panel
static M5Canvas mask2(&mask);    // mask to make VU-meter panel

enum palette { palette0 = 0, palette1, palette2, palette3 };
const int color_transparent(0);
      int color_background (0);
const int color_almostblack(0x0841);  //   8,   8,   8
const int color_darkred(0xc000);      // 192,   0,   0

void lcd_init()
{
  lcd.init();
  lcd.setRotation(1);
  lcd.setBrightness(64);
  lcd.clearDisplay();

  // create a sprite of meter panel
  panel.setColorDepth(2);
  panel.createSprite(320, 120);
}

void lcd_clear()
{
  lcd.clearDisplay();
}

// generate VU-meter panel into the sprite "panel"
void make_panel_vu()
{
  // panel color
  color_background  = 0xc408;  // 192, 128,  64
  canvas.setPaletteColor(palette1, color_background);

  // clear sprite
  panel.fillSprite(palette1);

  // draw minor ticks
  const float tick2[] = {
    0.878539761f,
    1.265524188f,
    1.420828251f,
  };
  int num_of_tick2 = sizeof(tick2) / sizeof(tick2[0]);
  for (int i = 0; i < num_of_tick2; ++i) {
    int x = 160.0f - 220.0f * cos(tick2[i]);
    int y = 200.0f - 220.0f * sin(tick2[i]);
    panel.drawLine(160, 160, x, y, palette2);
  }
  mask.setColorDepth(1);
  mask.createSprite(320, 120);
  mask.fillSprite(palette1);
  mask.fillCircle(160, 450, 413, palette0);
  mask.fillCircle(160, 450, 400, palette1);
  mask.pushSprite(0, 0, palette0);

  // draw major ticks
  const float tick1[] = {
    0.785398163f,
    1.044171546f,
    1.200296248f,
    1.338711140f,
    1.512965164f,
    1.616344482f,
    1.732337984f,
    1.862484834f,
    2.008512002f,  // red
    2.172357178f,  // red
    2.356194490f,  // red
  };
  int num_of_tick1 = sizeof(tick1) / sizeof(tick1[0]);
  int red_of_tick1 = num_of_tick1 - 3;
  for (int i = 0; i < num_of_tick1; ++i) {
    int x = 160.0f - 220.0f * cos(tick1[i]);
    int y = 200.0f - 220.0f * sin(tick1[i]);
    if (i < red_of_tick1) {
      panel.drawLine(160, 160, x,     y,     palette2);
      panel.drawLine(159, 161, x - 1, y + 1, palette2);
    }
    else {
      panel.drawLine(160, 160, x,     y,     palette3);
      panel.drawLine(161, 161, x + 1, y + 1, palette3);
    }
  }
  mask.fillSprite(palette1);
  mask.fillCircle(160, 450, 416, palette0);
  mask.fillCircle(160, 450, 400, palette1);
  mask.pushSprite(0, 0, palette0);
  mask.deleteSprite();

  // draw arcs
  panel.fillArc(160, 450, 400, 406, 276, 288, palette3);
  panel.fillArc(160, 450, 400, 403, 252, 276, palette2);

  // draw scale, etc.
  panel.setTextSize(1);
  panel.setTextDatum(MC_DATUM);  // Middle-Center

  panel.setFont(&fonts::FreeMono18pt7b);
  panel.setTextColor(palette2, palette1);
  panel.drawString("-",   20, 20);
  panel.setTextColor(palette3, palette1);
  panel.drawString("+",  300, 20);

  panel.setFont(&fonts::FreeSans12pt7b);
  panel.setTextColor(palette2, palette1);
  panel.drawString("VU", 160, 90);

  panel.setFont(&fonts::FreeSans9pt7b);
  panel.drawString("20",  17, 47);
  panel.drawString("10",  67, 33);
  panel.drawString("7",   96, 28);
  panel.drawString("5",  120, 24);
  panel.drawString("3",  148, 23);
  panel.drawString("2",  167, 23);
  panel.drawString("1",  186, 24);
  panel.drawString("0",  210, 27);

  panel.setTextColor(palette3, palette1);
  panel.drawString("1",  238, 32);
  panel.drawString("2",  268, 37);
  panel.drawString("3",  303, 47);

  // draw frame
  panel.drawRect(0, 0, 320, 120, palette2);
}

// generate Mcintosh-like panel into the sprite "panel"
void make_panel_mc()
{
  // panel color
  color_background  = 0x0218;  //   0,  64, 192
  canvas.setPaletteColor(palette1, color_background);

  // clear sprite
  panel.fillSprite(palette1);

  // draw ticks
  const float tick1[] = {
    0.785398163f,
    1.009797639f,
    1.234197114f,
    1.458596589f,
    1.682996064f,
    1.907395540f,
    2.131795015f,
    2.356194490f,
  };
  int num_of_tick1 = sizeof(tick1) / sizeof(tick1[0]);
  for (int i = 0; i < num_of_tick1; ++i) {
    int x = 160.0f - 220.0f * cos(tick1[i]);
    int y = 200.0f - 220.0f * sin(tick1[i]);
    if (i == 0) {
      panel.drawLine(160, 160, x,     y,     palette2);
      panel.drawLine(159, 161, x - 1, y + 1, palette2);
    }
    if (i == num_of_tick1 - 1) {
      panel.drawLine(160, 160, x,     y,     palette2);
      panel.drawLine(161, 161, x + 1, y + 1, palette2);
    }
  }
  mask.setColorDepth(1);
  mask.createSprite(320, 120);
  mask.fillSprite(palette1);
  mask.fillCircle(160, 450, 410, palette0);
  mask.fillCircle(160, 450, 400, palette1);
  mask.pushSprite(0, 0, palette0);

  // draw arc
  panel.fillArc(160, 450, 400, 406, 252, 288, palette2);

  // make mask for ticks on arc
  mask.fillSprite(palette0);
  for (int i = 1; i < num_of_tick1 - 1; ++i) {
    int x = 160.0f - 220.0f * cos(tick1[i]);
    int y = 200.0f - 220.0f * sin(tick1[i]);
    mask.drawLine(160, 160, x, y, palette1);
    if (i < num_of_tick1 / 2)
      mask.drawLine(159, 161, x - 1, y + 1, palette1);
    else
      mask.drawLine(161, 161, x + 1, y + 1, palette1);
  }
  mask2.setColorDepth(1);
  mask2.createSprite(320, 120);
  mask2.fillSprite(palette0);
  mask2.fillCircle(160, 450, 406, palette1);
  mask2.fillCircle(160, 450, 400, palette0);
  mask2.pushSprite(0, 0, palette1);
  mask2.deleteSprite();
  mask.pushSprite(0, 0, palette0);
  mask.deleteSprite();

  // draw scale, etc.
  panel.setTextSize(1);
  panel.setTextDatum(MC_DATUM);  // Middle-Center
  panel.setTextColor(palette2, palette1);
  panel.setFont(&fonts::Font2);
  panel.drawString("OUTPUT LEVEL", 160, 100);
  panel.drawString("DECIBELS", 160, 70);
  panel.drawString("-50",  70, 42);
  panel.drawString("-40", 107, 35);
  panel.drawString("-30", 145, 32);
  panel.drawString("-20", 180, 32);
  panel.drawString("-10", 218, 35);
  panel.drawString("0",   255, 42);

  // draw frame
  panel.drawRect(0, 0, 320, 120, palette2);
}

// add needles, push to the LCD
// 0VU = 1227mV (1227 in integer), 3VU = 1734mV
// level1: black needle
// level2: red needle
void show_meter(int location_x, int location_y, const char* left_right, int level1, int level2)
{
  const float pi(3.141592653589793f);
  const float vu_3(3468.0f);

  // create canvas
  canvas.setColorDepth(2);
  canvas.setPaletteColor(palette0, color_transparent);
  canvas.setPaletteColor(palette1, color_background);
  canvas.setPaletteColor(palette2, color_almostblack);
  canvas.setPaletteColor(palette3, color_darkred);
  canvas.createSprite(320, 120);

  // set background of VU-meter
  panel.pushSprite(0, 0);

  // add "L"/"R"
  canvas.setTextSize(1);
  canvas.setTextDatum(MC_DATUM);  // Middle-Center
  canvas.setFont(&fonts::FreeMono12pt7b);
  canvas.setTextColor(palette2, palette1);
  canvas.drawString(left_right, 20, 100);

  // needle red
  if (level2 >= 0) {
    float a = (0.25f + (level2 / vu_3)) * pi;
    int   x = 160.0f - 190.0f * cos(a);
    int   y = 200.0f - 190.0f * sin(a);
    canvas.drawLine(161, 160, x + 1, y,     palette3);
    canvas.drawLine(159, 160, x - 1, y,     palette3);
    canvas.drawLine(160, 161, x,     y + 1, palette3);
    canvas.drawLine(160, 159, x,     y - 1, palette3);
  }

  // needle black
  float a = (0.25f + (level1 / vu_3)) * pi;
  int   x = 160.0f - 190.0f * cos(a);
  int   y = 200.0f - 190.0f * sin(a);
  canvas.drawLine(161, 160, x + 1, y,     palette2);
  canvas.drawLine(159, 160, x - 1, y,     palette2);
  canvas.drawLine(160, 161, x,     y + 1, palette2);
  canvas.drawLine(160, 159, x,     y - 1, palette2);

  // put into the LCD
  canvas.pushSprite(location_x, location_y);
  canvas.deleteSprite();
}
