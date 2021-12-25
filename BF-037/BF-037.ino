// Copyright 2021 BotanicFields, Inc.
// VU meter on M5Stack

#include <M5Stack.h>
#include "esp_adc_cal.h"   // for esp_adc_cal_characteristics_t
#include "BF_M5StackVuMeter.h"

// for control loop
const int loop_ms(50);  // 50ms
unsigned int last_ms(0);

// for analog to degital converter
esp_adc_cal_characteristics_t adc_chars;

const int offset(142);  // offset 142mV at ground-level on ESP32 ADC 
const int vu_0(1227);   // 0VU = 1227mV
const int vu_diff   = vu_0 / ( 300 / loop_ms);  //  300ms for settling from -20VU to 0VU
const int peak_diff = vu_0 / (8000 / loop_ms);  // 8000ms for recovery from 0VU to -20VU
int last_vu_r(0);
int last_vu_l(0);
int last_peak_r(0);
int last_peak_l(0);

void setup()
{
  const bool lcd_enable(true);
  const bool sd_enable(true);
  const bool serial_enable(true);
  const bool i2c_enable(true);
  M5.begin(!lcd_enable, !sd_enable, serial_enable, i2c_enable);

  // analog to digital converter
  adc_gpio_init(ADC_UNIT_1, ADC_CHANNEL_7);                    // ADC1 Channel 7 = GPIO35
  adc_gpio_init(ADC_UNIT_1, ADC_CHANNEL_0);                    // ADC1 Channel 0 = GPIO36
  adc1_config_channel_atten(ADC1_CHANNEL_7, ADC_ATTEN_DB_11);  // attenuation 11dB
  adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11);  // attenuation 11dB
  adc1_config_width(ADC_WIDTH_BIT_12);                         // width 12bit

  //Characterize ADC at particular ATTEN
  const int default_vref(1100);
  esp_adc_cal_value_t cal_value = esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, default_vref, &adc_chars);

  //Check type of calibration value used to characterize ADC
  String msg = "[AdcWaveInit] cal_value = ";
  switch (cal_value) {
  case ESP_ADC_CAL_VAL_EFUSE_VREF: msg += "eFuse Vref"; break;
  case ESP_ADC_CAL_VAL_EFUSE_TP:   msg += "Two Point";  break;
  default:                         msg += "Default";    break;
  }
  Serial.println(msg);

  make_vu_panel();

  last_ms = millis();
}

void loop()
{
  M5.update();

  int adc_7 = esp_adc_cal_raw_to_voltage(adc1_get_raw(ADC1_CHANNEL_7), &adc_chars);
  int adc_0 = esp_adc_cal_raw_to_voltage(adc1_get_raw(ADC1_CHANNEL_0), &adc_chars);

  // for ESP32 ADC
  adc_7 = (adc_7 - offset) * vu_0 / (vu_0 - offset);
  adc_0 = (adc_0 - offset) * vu_0 / (vu_0 - offset);

  // response
  if (adc_7 > last_vu_l   + vu_diff)   last_vu_l   += vu_diff;   else last_vu_l   = adc_7;
  if (adc_0 > last_vu_r   + vu_diff)   last_vu_r   += vu_diff;   else last_vu_r   = adc_0;
  if (adc_7 < last_peak_l - peak_diff) last_peak_l -= peak_diff; else last_peak_l = adc_7;
  if (adc_0 < last_peak_r - peak_diff) last_peak_r -= peak_diff; else last_peak_r = adc_0;

  show_vu_meter(0,   0, "L", last_vu_l, last_peak_l);
  show_vu_meter(0, 120, "R", last_vu_r, last_peak_r);

  Serial.printf("elapse=%dms adc7=%d adc0=%d\n", millis() - last_ms, adc_7, adc_0);  // elapsed time
  delay(last_ms + loop_ms - millis());
  last_ms = millis();
}
