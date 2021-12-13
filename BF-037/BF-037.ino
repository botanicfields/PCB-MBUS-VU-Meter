// Copyright 2021 BotanicFields, Inc.
// VU meter on M5Stack

#include <M5Stack.h>
#include "esp_adc_cal.h"   // for esp_adc_cal_characteristics_t
#include "BF_M5StackVuMeter.h"

// for analog to degital converter
esp_adc_cal_characteristics_t adc_chars;

// for control loop
const int loop_ms(50);  // 50ms
int last_ms(0);

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

  show_vu_meter(0,   0, "L", adc_7);
  show_vu_meter(0, 120, "R", adc_0);

  Serial.printf("elapse=%dms adc7=%d adc0=%d\n", millis() - last_ms, adc_7, adc_0);  // elapsed time
  delay(last_ms + loop_ms - millis());
  last_ms = millis();
}
