/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    adc.h
  * @brief   This file contains all the function prototypes for
  *          the adc.c file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ADC_H__
#define __ADC_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern ADC_HandleTypeDef hadc1;

/* USER CODE BEGIN Private defines */

enum AdcReading {
    ADC_READING_SENSE_5V = 0,
    ADC_READING_BSPS_FRONT,
    ADC_READING_BSPS_REAR,
    ADC_READING_BOTS,
    ADC_READING_BPPS,
    ADC_READING_APPS_3,
    ADC_READING_APPS_2,
    ADC_READING_APPS_1,
    ADC_READING_COUNT
};

/* USER CODE END Private defines */

void MX_ADC1_Init(void);

/* USER CODE BEGIN Prototypes */

void adc_init(void);

uint16_t adc_read_raw(enum AdcReading reading);

char *adc_get_reading_name(enum AdcReading reading);

float adc_read_voltage(enum AdcReading reading);

void adc_update_modules();

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __ADC_H__ */
