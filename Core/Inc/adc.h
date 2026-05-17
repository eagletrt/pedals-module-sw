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
#define ADC_READING_MIN_RAW_VALUE (0.0F)
#define ADC_READING_MAX_RAW_VALUE (4095.0F)

/*!
 * \brief used to identify the different values read in the adc module
 * 
 */
enum ADCReadingSensorName {
    ADC_READING_SENSOR_NAME_SENSE_5V,
    ADC_READING_SENSOR_NAME_BSPS_F,
    ADC_READING_SENSOR_NAME_BSPS_R,
    ADC_READING_SENSOR_NAME_BPPS,
    ADC_READING_SENSOR_NAME_APPS_3,
    ADC_READING_SENSOR_NAME_APPS_2,
    ADC_READING_SENSOR_NAME_APPS_1,
    ADC_READING_SENSOR_NAME_NUMBER_OF_SENSORS
};
/* USER CODE END Private defines */

void MX_ADC1_Init(void);

/* USER CODE BEGIN Prototypes */
/*!
 * \brief Given an interval, it returns the value read in the adc module as a percentage of the interval 
 * 
 * \param sensor name of the sensor of which you want the percentage of
 * \param normal_min lower bound of the interval
 * \param normal_max upper bound of the interval
 * \return float ideally in range [0,1], but can be outside depending on normal_min, normal_max and the values read in the adc module
 */
float adc_reading_return_percentage(enum ADCReadingSensorName sensor, int32_t normal_min, int32_t normal_max);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __ADC_H__ */
