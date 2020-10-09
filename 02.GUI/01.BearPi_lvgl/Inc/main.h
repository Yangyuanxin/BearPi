/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
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
#include "stm32l4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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
#define BOARD_LED_Pin GPIO_PIN_13
#define BOARD_LED_GPIO_Port GPIOC
#define WIFI_RX_Pin GPIO_PIN_0
#define WIFI_RX_GPIO_Port GPIOC
#define WIFI_TX_Pin GPIO_PIN_1
#define WIFI_TX_GPIO_Port GPIOC
#define LCD_MOSI_Pin GPIO_PIN_3
#define LCD_MOSI_GPIO_Port GPIOC
#define LCD_SCK_Pin GPIO_PIN_13
#define LCD_SCK_GPIO_Port GPIOB
#define LCD_POWER_Pin GPIO_PIN_15
#define LCD_POWER_GPIO_Port GPIOB
#define LCD_WR_RS_Pin GPIO_PIN_6
#define LCD_WR_RS_GPIO_Port GPIOC
#define LCD_RESET_Pin GPIO_PIN_7
#define LCD_RESET_GPIO_Port GPIOC
#define LED_Pin GPIO_PIN_9
#define LED_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */
void plot_display_controld(uint8_t flag);
void LED_BLINK_controld(uint8_t flag);
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
