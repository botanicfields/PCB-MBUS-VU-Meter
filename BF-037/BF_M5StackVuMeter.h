// Copyright 2022 BotanicFields, Inc.
// BF-037 VU meters on M5Stack

#pragma once

void lcd_init();
void lcd_clear();
void make_panel_vu();
void make_panel_mc();
void show_meter(int location_x, int location_y, const char* left_right, int level1, int level2 = -1);
