/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32c0xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define USER_LED_Pin GPIO_PIN_0
#define USER_LED_GPIO_Port GPIOA
#define SENSE_5V_Pin GPIO_PIN_1
#define SENSE_5V_GPIO_Port GPIOA
#define BSPS_F_Pin GPIO_PIN_2
#define BSPS_F_GPIO_Port GPIOA
#define BSPS_R_Pin GPIO_PIN_3
#define BSPS_R_GPIO_Port GPIOA
#define BOTS_Pin GPIO_PIN_4
#define BOTS_GPIO_Port GPIOA
#define BOTS_EXTI_IRQn EXTI4_15_IRQn
#define BPPS_Pin GPIO_PIN_5
#define BPPS_GPIO_Port GPIOA
#define APPS_3_Pin GPIO_PIN_6
#define APPS_3_GPIO_Port GPIOA
#define APPS_2_Pin GPIO_PIN_7
#define APPS_2_GPIO_Port GPIOA
#define APPS_1_Pin GPIO_PIN_8
#define APPS_1_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
