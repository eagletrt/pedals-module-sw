#ifndef ADC_UTILS_H
#define ADC_UTILS_H

#include "stdbool.h"
#include "stdint.h"

typedef enum{
	ADC_CHANNEL_APPS_1 = 0,
	ADC_CHANNEL_APPS_2,
	ADC_CHANNEL_APPS_3,
	ADC_CHANNEL_BPPS,
	ADC_CHANNEL_BSPS_F,
	ADC_CHANNEL_BSPS_R,
	ADC_CHANNEL_5V_SENSE
} ADC_Channel_Typedef;

uint32_t read_from_raw(ADC_Channel_Typedef channel);

#endif