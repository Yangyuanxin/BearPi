/**
  ******************************************************************************
  * File Name          : RTC.h
  * Description        : This file provides code for the configuration
  *                      of the RTC instances.
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __rtc_H
#define __rtc_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern RTC_HandleTypeDef hrtc;

/* USER CODE BEGIN Private defines */
typedef struct 
{
    uint16_t year   ;		//年
    uint8_t month   ; 	//月
    uint8_t day     ;		//日
    uint8_t hour    ;		//时
    uint8_t minute  ;		//分
		uint8_t sec 		;		//秒
		char DisPlay_DateBuf[20];
		char DisPlay_TimeBuf[20];
} DateTime_HandleTypeDef;
extern DateTime_HandleTypeDef DateTime_Handler_Info ;
/* USER CODE END Private defines */

void MX_RTC_Init(void);

/* USER CODE BEGIN Prototypes */
//获取日期和时间
void Get_Date_Time(void);
HAL_StatusTypeDef RTC_Set_Time(uint8_t hour, uint8_t min, uint8_t sec, uint8_t ampm);
HAL_StatusTypeDef RTC_Set_Date(uint8_t year, uint8_t month, uint8_t date, uint8_t week);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ rtc_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
