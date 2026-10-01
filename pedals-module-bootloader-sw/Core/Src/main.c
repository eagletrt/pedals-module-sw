/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "stm32c0xx_hal.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "boot.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
#include <string.h>

static void DbgReg(const char *name, uint32_t v) {
    static const char hex[] = "0123456789ABCDEF";
    char buf[10];
    for (int i = 0; i < 8; i++) {
        buf[i] = hex[(v >> (28 - 4 * i)) & 0xF];
    }
    buf[8] = '\r';
    buf[9] = '\n';
    HAL_UART_Transmit(&huart1, (uint8_t *)name, strlen(name), 20);
    HAL_UART_Transmit(&huart1, (uint8_t *)buf, 10, 20);
}
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void) {

    /* USER CODE BEGIN 1 */

    /* USER CODE END 1 */

    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();

    /* USER CODE BEGIN SysInit */

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_USART1_UART_Init();
    /* USER CODE BEGIN 2 */
    /* FDCAN1 is initialized by OpenBLT's CanInit() (can.c), which computes the bit timing
   * from BOOT_COM_CAN_BAUDRATE and reuses HAL_FDCAN_MspInit() from fdcan.c for the pins
   * and kernel clock. MX_FDCAN1_Init() is therefore not called.
   */

    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */

    BootInit();

    HAL_UART_Transmit(&huart1, (uint8_t *)"Init Done", 10, 20);

    while (1) {
        BootTask();

        static uint32_t last;
        if (HAL_GetTick() - last >= 1000) {
            last = HAL_GetTick();
            DbgReg("CFGR1 ", SYSCFG->CFGR1);
            DbgReg("CCCR  ", FDCAN1->CCCR);
            DbgReg("PSR   ", FDCAN1->PSR);
            DbgReg("ECR   ", FDCAN1->ECR);
            DbgReg("RXF0S ", FDCAN1->RXF0S);
        }
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
    }
    /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void) {

    LL_FLASH_SetLatency(LL_FLASH_LATENCY_1);

    /* HSI configuration and activation */
    LL_RCC_HSI_Enable();
    while (LL_RCC_HSI_IsReady() != 1) {
    }

    LL_RCC_HSI_SetCalibTrimming(64);
    LL_RCC_SetHSIDiv(LL_RCC_HSI_DIV_1);
    /* Set AHB prescaler*/
    LL_RCC_SetAHBPrescaler(LL_RCC_HCLK_DIV_1);

    LL_RCC_SetSYSDivider(LL_RCC_SYSCLK_DIV_1);
    /* Sysclk activation on the HSI */
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSI);
    while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSI) {
    }

    /* Set APB1 prescaler*/
    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
    /* Update CMSIS variable (which can be updated also through SystemCoreClockUpdate function) */
    LL_SetSystemCoreClock(48000000);

    /* Update the time base */
    if (HAL_InitTick(TICK_INT_PRIORITY) != HAL_OK) {
        Error_Handler();
    }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void) {
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1) {
    }
    /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line) {
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
