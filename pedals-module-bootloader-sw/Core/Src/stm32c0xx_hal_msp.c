/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file         stm32c0xx_hal_msp.c
  * @brief        This file provides code for the MSP Initialization
  *               and de-Initialization codes.
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
#include "main.h"
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN Define */

/* USER CODE END Define */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN Macro */

/* USER CODE END Macro */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* External functions --------------------------------------------------------*/
/* USER CODE BEGIN ExternalFunctions */

/* USER CODE END ExternalFunctions */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */
/**
  * Initializes the Global MSP.
  */
void HAL_MspInit(void)
{

  /* USER CODE BEGIN MspInit 0 */

  /* USER CODE END MspInit 0 */

  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();

  /* System interrupt init*/

  /* USER CODE BEGIN MspInit 1 */

  /* USER CODE END MspInit 1 */
}

/* USER CODE BEGIN 1 */

/**
  * Called by HAL_DeInit() in CpuStartUserProgram() right before jumping to the main
  * firmware. Hands over the MCU as close as possible to its reset state, like the
  * official OpenBLT STM32C0 demo does, so the main firmware starts from a clean
  * FDCAN peripheral and its own clock configuration.
  */
void HAL_MspDeInit(void)
{
  /* OpenBLT's ComFree() does not stop the CAN controller, so reset it here together
   * with its GPIO port (PA11/PA12). The bootloader does not use any other pin of
   * port A, so resetting the whole port is equivalent to HAL_GPIO_DeInit() but smaller.
   */
  __HAL_RCC_FDCAN1_FORCE_RESET();
  __HAL_RCC_FDCAN1_RELEASE_RESET();
  __HAL_RCC_FDCAN1_CLK_DISABLE();
  __HAL_RCC_GPIOA_FORCE_RESET();
  __HAL_RCC_GPIOA_RELEASE_RESET();
  __HAL_RCC_GPIOA_CLK_DISABLE();

  /* Reset the RCC clock configuration to the default reset state. The LL version is used
   * because HAL_RCC_DeInit() would also re-arm the SysTick through HAL_InitTick().
   */
  LL_RCC_DeInit();

  __HAL_RCC_PWR_CLK_DISABLE();
  __HAL_RCC_SYSCFG_CLK_DISABLE();
}

/* USER CODE END 1 */
