// Copyright 2021 BotanicFields, Inc.
// VU meter on M5Stack

#include <M5GFX.h>
#include "BF_M5StackVuMeter.h"

static M5GFX lcd;
static M5Canvas canvas(&lcd);    // final image of VU meter
static M5Canvas panel(&canvas);  // panel image of VU meter
static M5Canvas mask(&panel);    // mask to make VU-meter panel

enum palette { palette0 = 0, palette1, palette2, palette3 };
const int color_transparent(0);
const int color_background (0xc408);  // 192, 128,  64
const int color_almostblack(0x0841);  //   8,   8,   8
const int color_darkred    (0xc000);  // 192,   0,   0

// generate vu-meter panel into the sprite "panel"
void make_vu_panel()
{
  lcd.init();
  lcd.setRotation(1);
  lcd.setBrightness(64);

  // create a sprite of VU-meter panel
  panel.setColorDepth(2);
  panel.createSprite(320, 120);
  panel.fillSprite(palette1);

  // create a sprite for masking
  mask.setColorDepth(1);
  mask.createSprite(320, 120);

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
  mask.fillSprite(palette1);
  mask.fillCircle(160, 450, 414, palette0);
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
    2.008512002f,
    2.172357178f,
    2.356194490f,
  };
  int num_of_tick1 = sizeof(tick1) / sizeof(tick1[0]);
  int red_of_tick1 = num_of_tick1 - 3;
  for (int i = 0; i < red_of_tick1; ++i) {
    int x = 160.0f - 220.0f * cos(tick1[i]);
    int y = 200.0f - 220.0f * sin(tick1[i]);
    panel.drawLine(160, 160, x,     y,     palette2);
    panel.drawLine(159, 161, x - 1, y + 1, palette2);
  }
  for (int i = red_of_tick1; i < num_of_tick1; ++i) {
    int x = 160.0f - 220.0f * cos(tick1[i]);
    int y = 200.0f - 220.0f * sin(tick1[i]);
    panel.drawLine(160, 160, x,     y,     palette3);
    panel.drawLine(161, 161, x + 1, y + 1, palette3);
  }
  mask.fillSprite(palette1);
  mask.fillCircle(160, 450, 418, palette0);
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
  panel.drawString("20",  15, 45);
  panel.drawString("10",  65, 31);
  panel.drawString("7",   95, 26);
  panel.drawString("5",  120, 22);
  panel.drawString("3",  147, 21);
  panel.drawString("2",  167, 21);
  panel.drawString("1",  186, 22);
  panel.drawString("0",  210, 25);
  panel.setTextColor(palette3, palette1);
  panel.drawString("1",  240, 30);
  panel.drawString("2",  270, 35);
  panel.drawString("3",  305, 45);

  // draw frame
  panel.drawRect(0, 0, 320, 120, palette2);
}

// add needles, push to the LCD
// 0VU = 1227mV (1227 in integer), 3VU = 1734mV
// level1: black needle
// level2: red needle
void show_vu_meter(int location_x, int location_y, const char* left_right, int level1, int level2)
{
  const float pi(3.141592653589793f);

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

  // needle black
  float a = (0.25f + (level1 / 3468.0f)) * pi;
  int x = 160.0f - 190.0f * cos(a);
  int y = 200.0f - 190.0f * sin(a);
  canvas.drawLine(161, 160, x + 1, y,     palette2);
  canvas.drawLine(159, 160, x - 1, y,     palette2);
  canvas.drawLine(160, 161, x,     y + 1, palette2);
  canvas.drawLine(160, 159, x,     y - 1, palette2);

  // needle red
  if (level2 >= 0) {
    a = (0.25f + (level2 / 3468.0f)) * pi;
    x = 160.0f - 190.0f * cos(a);
    y = 200.0f - 190.0f * sin(a);
    canvas.drawLine(161, 160, x + 1, y,     palette3);
    canvas.drawLine(159, 160, x - 1, y,     palette3);
    canvas.drawLine(160, 161, x,     y + 1, palette3);
    canvas.drawLine(160, 159, x,     y - 1, palette3);
  }
  // put into the LCD
  canvas.pushSprite(location_x, location_y);
  canvas.deleteSprite();
}
