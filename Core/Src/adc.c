/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    adc.c
  * @brief   This file provides code for the configuration
  *          of the ADC instances.
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
/* Includes ------------------------------------------------------------------*/
#include "adc.h"

/* USER CODE BEGIN 0 */

#include <string.h>
#include "brake-api.h"
#include "throttle-api.h"
#include "eagletrt-api.h"
#include "adc_conversion.h"

constexpr int ADC_BUFFER_SIZE = 8U;

uint16_t adc_buffer[ADC_BUFFER_SIZE];
uint16_t adc_values[ADC_BUFFER_SIZE];

/* USER CODE END 0 */

ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

/* ADC1 init function */
void MX_ADC1_Init(void) {

    /* USER CODE BEGIN ADC1_Init 0 */

    /* USER CODE END ADC1_Init 0 */

    ADC_ChannelConfTypeDef sConfig = { 0 };

    /* USER CODE BEGIN ADC1_Init 1 */

    /* USER CODE END ADC1_Init 1 */

    /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
    hadc1.Instance = ADC1;
    hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
    hadc1.Init.Resolution = ADC_RESOLUTION_12B;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.ScanConvMode = ADC_SCAN_ENABLE;
    hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    hadc1.Init.LowPowerAutoWait = DISABLE;
    hadc1.Init.LowPowerAutoPowerOff = DISABLE;
    hadc1.Init.ContinuousConvMode = DISABLE;
    hadc1.Init.NbrOfConversion = 8;
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConv = ADC_EXTERNALTRIG_T3_TRGO;
    hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
    hadc1.Init.DMAContinuousRequests = ENABLE;
    hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
    hadc1.Init.SamplingTimeCommon1 = ADC_SAMPLETIME_12CYCLES_5;
    hadc1.Init.SamplingTimeCommon2 = ADC_SAMPLETIME_12CYCLES_5;
    hadc1.Init.OversamplingMode = DISABLE;
    hadc1.Init.TriggerFrequencyMode = ADC_TRIGGER_FREQ_HIGH;
    if (HAL_ADC_Init(&hadc1) != HAL_OK) {
        Error_Handler();
    }

    /** Configure Regular Channel
  */
    sConfig.Channel = ADC_CHANNEL_1;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLINGTIME_COMMON_1;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
        Error_Handler();
    }

    /** Configure Regular Channel
  */
    sConfig.Channel = ADC_CHANNEL_2;
    sConfig.Rank = ADC_REGULAR_RANK_2;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
        Error_Handler();
    }

    /** Configure Regular Channel
  */
    sConfig.Channel = ADC_CHANNEL_3;
    sConfig.Rank = ADC_REGULAR_RANK_3;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
        Error_Handler();
    }

    /** Configure Regular Channel
  */
    sConfig.Channel = ADC_CHANNEL_4;
    sConfig.Rank = ADC_REGULAR_RANK_4;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
        Error_Handler();
    }

    /** Configure Regular Channel
  */
    sConfig.Channel = ADC_CHANNEL_5;
    sConfig.Rank = ADC_REGULAR_RANK_5;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
        Error_Handler();
    }

    /** Configure Regular Channel
  */
    sConfig.Channel = ADC_CHANNEL_6;
    sConfig.Rank = ADC_REGULAR_RANK_6;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
        Error_Handler();
    }

    /** Configure Regular Channel
  */
    sConfig.Channel = ADC_CHANNEL_7;
    sConfig.Rank = ADC_REGULAR_RANK_7;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
        Error_Handler();
    }

    /** Configure Regular Channel
  */
    sConfig.Channel = ADC_CHANNEL_8;
    sConfig.Rank = ADC_REGULAR_RANK_8;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
        Error_Handler();
    }
    /* USER CODE BEGIN ADC1_Init 2 */

    /* USER CODE END ADC1_Init 2 */
}

void HAL_ADC_MspInit(ADC_HandleTypeDef *adcHandle) {

    GPIO_InitTypeDef GPIO_InitStruct = { 0 };
    RCC_PeriphCLKInitTypeDef PeriphClkInit = { 0 };
    if (adcHandle->Instance == ADC1) {
        /* USER CODE BEGIN ADC1_MspInit 0 */

        /* USER CODE END ADC1_MspInit 0 */

        /** Initializes the peripherals clocks
  */
        PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
        PeriphClkInit.AdcClockSelection = RCC_ADCCLKSOURCE_SYSCLK;
        if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
            Error_Handler();
        }

        /* ADC1 clock enable */
        __HAL_RCC_ADC_CLK_ENABLE();

        __HAL_RCC_GPIOA_CLK_ENABLE();
        /**ADC1 GPIO Configuration
    PA1     ------> ADC1_IN1
    PA2     ------> ADC1_IN2
    PA3     ------> ADC1_IN3
    PA4     ------> ADC1_IN4
    PA5     ------> ADC1_IN5
    PA6     ------> ADC1_IN6
    PA7     ------> ADC1_IN7
    PA8     ------> ADC1_IN8
    */
        GPIO_InitStruct.Pin = SENSE_5V_Pin | BSPS_FRONT_Pin | BSPS_REAR_Pin | BOTS_Pin | BPPS_Pin | APPS_3_Pin | APPS_2_Pin | APPS_1_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        /* ADC1 DMA Init */
        /* ADC1 Init */
        hdma_adc1.Instance = DMA1_Channel1;
        hdma_adc1.Init.Request = DMA_REQUEST_ADC1;
        hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
        hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_adc1.Init.MemInc = DMA_MINC_ENABLE;
        hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
        hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
        hdma_adc1.Init.Mode = DMA_CIRCULAR;
        hdma_adc1.Init.Priority = DMA_PRIORITY_LOW;
        if (HAL_DMA_Init(&hdma_adc1) != HAL_OK) {
            Error_Handler();
        }

        __HAL_LINKDMA(adcHandle, DMA_Handle, hdma_adc1);

        /* USER CODE BEGIN ADC1_MspInit 1 */

        /* USER CODE END ADC1_MspInit 1 */
    }
}

void HAL_ADC_MspDeInit(ADC_HandleTypeDef *adcHandle) {

    if (adcHandle->Instance == ADC1) {
        /* USER CODE BEGIN ADC1_MspDeInit 0 */

        /* USER CODE END ADC1_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_ADC_CLK_DISABLE();

        /**ADC1 GPIO Configuration
    PA1     ------> ADC1_IN1
    PA2     ------> ADC1_IN2
    PA3     ------> ADC1_IN3
    PA4     ------> ADC1_IN4
    PA5     ------> ADC1_IN5
    PA6     ------> ADC1_IN6
    PA7     ------> ADC1_IN7
    PA8     ------> ADC1_IN8
    */
        HAL_GPIO_DeInit(GPIOA, SENSE_5V_Pin | BSPS_FRONT_Pin | BSPS_REAR_Pin | BOTS_Pin | BPPS_Pin | APPS_3_Pin | APPS_2_Pin | APPS_1_Pin);

        /* ADC1 DMA DeInit */
        HAL_DMA_DeInit(adcHandle->DMA_Handle);
        /* USER CODE BEGIN ADC1_MspDeInit 1 */

        /* USER CODE END ADC1_MspDeInit 1 */
    }
}

/* USER CODE BEGIN 1 */

#define SENSE_5V_V_DIVIDER(read) ((read)*3.3f / 4095.0f / (18.0f / (11.8f + 18.0f)))
#define BSPS_FRONT_V_DIVIDER(read) ((read)*3.3f / 4095.0f / (18.0f / (11.8f + 18.0f)) * 1.005f) // last value is a calibration value referring to resistance
#define BSPS_REAR_V_DIVIDER(read) ((read)*3.3f / 4095.0f / (18.0f / (11.8f + 18.0f)) * 1.006f)  // same as above
#define BOTS_V_DIVIDER(read) ((read)*3.3f / 4095.0f / (47.0f / (330.0f + 47.0f)))
#define BPPS_V_DIVIDER(read) ((read)*3.3f / 4095.0f / (18.0f / (11.8f + 18.0f)))
#define APPS_3_V_DIVIDER(read) ((read)*3.3f / 4095.0f / (18.0f / (11.8f + 18.0f)))
#define APPS_2_V_DIVIDER(read) ((read)*3.3f / 4095.0f / (18.0f / (11.8f + 18.0f)))
#define APPS_1_V_DIVIDER(read) ((read)*3.3f / 4095.0f / (18.0f / (11.8f + 18.0f)))

void adc_init(void) {
    HAL_ADCEx_Calibration_Start(&hadc1);
    HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc_buffer, ADC_BUFFER_SIZE);
}

char *adc_get_reading_name(enum AdcReading reading) {
    switch (reading) {
        case ADC_READING_SENSE_5V:
            return "SENSE_5V";
        case ADC_READING_BSPS_FRONT:
            return "BSPS_FRONT";
        case ADC_READING_BSPS_REAR:
            return "BSPS_REAR";
        case ADC_READING_BOTS:
            return "BOTS";
        case ADC_READING_BPPS:
            return "BPPS";
        case ADC_READING_APPS_3:
            return "APPS_3";
        case ADC_READING_APPS_2:
            return "APPS_2";
        case ADC_READING_APPS_1:
            return "APPS_1";
        default:
            return "UNKNOWN";
    }
}

uint16_t adc_read_raw(enum AdcReading reading) {
    if (reading >= ADC_READING_COUNT) {
        return 0;
    }
    return adc_values[reading];
}

float adc_read_voltage(enum AdcReading reading) {
    switch (reading) {
        case ADC_READING_SENSE_5V:
            return SENSE_5V_V_DIVIDER(adc_values[reading]);
        case ADC_READING_BSPS_FRONT:
            return BSPS_FRONT_V_DIVIDER(adc_values[reading]);
        case ADC_READING_BSPS_REAR:
            return BSPS_REAR_V_DIVIDER(adc_values[reading]);
        case ADC_READING_BOTS:
            return BOTS_V_DIVIDER(adc_values[reading]);
        case ADC_READING_BPPS:
            return BPPS_V_DIVIDER(adc_values[reading]);
        case ADC_READING_APPS_3:
            return APPS_3_V_DIVIDER(adc_values[reading]);
        case ADC_READING_APPS_2:
            return APPS_2_V_DIVIDER(adc_values[reading]);
        case ADC_READING_APPS_1:
            return APPS_1_V_DIVIDER(adc_values[reading]);
        default:
            return 0.0f;
    }
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
    if (hadc->Instance != ADC1) {
        return;
    }
    memcpy(adc_values, adc_buffer, sizeof(adc_values));

    float apps1 = ADC_CONV_APPS_1_RAW2VOLT(adc_values[ADC_READING_APPS_1]);
    float apps2 = ADC_CONV_APPS_2_RAW2VOLT(adc_values[ADC_READING_APPS_2]);
    float bpps = ADC_CONV_BPPS_RAW2VOLT(adc_values[ADC_READING_BPPS]);
    float front = ADC_CONV_BSPS_FRONT_RAW2VOLT(adc_values[ADC_READING_BSPS_FRONT]);
    float rear = ADC_CONV_BSPS_REAR_RAW2VOLT(adc_values[ADC_READING_BSPS_REAR]);
    //float bots = ADC_CONV_BOTS_RAW2VOLT(adc_values[ADC_READING_BOTS]);

    apps1 = ADC_CONV_APPS1_NORMALIZE(apps1);
    apps2 = ADC_CONV_APPS2_NORMALIZE(apps2);
    bpps = ADC_CONV_BPPS_NORMALIZE(bpps);
    front = ADC_CONV_BSPS_VOLT2BAR(front);
    rear = ADC_CONV_BSPS_VOLT2BAR(rear);
    throttle_api_update_pedal_values(apps1, apps2, -1.0f);
    brake_api_update_pedal_travel_percentage(bpps);
    brake_api_update_front_pressure(front);
    brake_api_update_rear_pressure(rear);
}

/* USER CODE END 1 */
