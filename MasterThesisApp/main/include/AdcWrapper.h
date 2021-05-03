//
// Created by lukaszk on 03.05.2021.
//

#ifndef MASTERTHESISAPP_ADCWRAPPER_H
#define MASTERTHESISAPP_ADCWRAPPER_H

#include <driver/adc.h>
#include "esp_adc_cal.h"

//auto* adc_chars = static_cast<esp_adc_cal_characteristics_t*>(
//    malloc(sizeof(esp_adc_cal_characteristics_t)));
//auto adc = new esp_adc_cal_characteristics_t;

void ConfigureADC(esp_adc_cal_characteristics_t *adc_params){
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_5, ADC_ATTEN_DB_11);
//    auto* adc_chars = static_cast<esp_adc_cal_characteristics_t*>(
//        malloc(sizeof(esp_adc_cal_characteristics_t)));
    esp_adc_cal_characterize(ADC_UNIT_1,
                             ADC_ATTEN_DB_11,
                             ADC_WIDTH_BIT_12,
                             ESP_ADC_CAL_VAL_EFUSE_VREF,
                             adc_params);
}

#endif // MASTERTHESISAPP_ADCWRAPPER_H
