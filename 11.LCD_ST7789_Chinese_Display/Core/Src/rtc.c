/**
  ******************************************************************************
  * File Name          : RTC.c
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

/* Includes ------------------------------------------------------------------*/
#include "rtc.h"

/* USER CODE BEGIN 0 */
RTC_TimeTypeDef RTC_TimeStruct;
RTC_DateTypeDef RTC_DateStruct;
DateTime_HandleTypeDef DateTime_Handler_Info ;
/* USER CODE END 0 */

RTC_HandleTypeDef hrtc;

/* RTC init function */
void MX_RTC_Init(void)
{

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

}

void HAL_RTC_MspInit(RTC_HandleTypeDef* rtcHandle)
{

  if(rtcHandle->Instance==RTC)
  {
  /* USER CODE BEGIN RTC_MspInit 0 */

  /* USER CODE END RTC_MspInit 0 */
    /* RTC clock enable */
    __HAL_RCC_RTC_ENABLE();
  /* USER CODE BEGIN RTC_MspInit 1 */

  /* USER CODE END RTC_MspInit 1 */
  }
}

void HAL_RTC_MspDeInit(RTC_HandleTypeDef* rtcHandle)
{

  if(rtcHandle->Instance==RTC)
  {
  /* USER CODE BEGIN RTC_MspDeInit 0 */

  /* USER CODE END RTC_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_RTC_DISABLE();
  /* USER CODE BEGIN RTC_MspDeInit 1 */

  /* USER CODE END RTC_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
//获取日期和时间
void Get_Date_Time(void)
{
    HAL_RTC_GetTime(&hrtc, &RTC_TimeStruct, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &RTC_DateStruct, RTC_FORMAT_BIN);
    DateTime_Handler_Info.year = RTC_DateStruct.Year + 2000;
    DateTime_Handler_Info.month = RTC_DateStruct.Month ;
    DateTime_Handler_Info.day = RTC_DateStruct.Date ;
    DateTime_Handler_Info.hour =  RTC_TimeStruct.Hours ;
    DateTime_Handler_Info.minute  =  RTC_TimeStruct.Minutes ;
		DateTime_Handler_Info.sec		= RTC_TimeStruct.Seconds;
}

/*
	RTC时间设置
	hour,min,sec:小时,分钟,秒钟
	ampm:@RTC_AM_PM_Definitions:RTC_HOURFORMAT12_AM/RTC_HOURFORMAT12_PM
	返回值:SUCEE(1),成功
       ERROR(0),进入初始化模式失败
*/
HAL_StatusTypeDef RTC_Set_Time(uint8_t hour, uint8_t min, uint8_t sec, uint8_t ampm)
{
    RTC_TimeTypeDef RTC_TimeStructure;

    RTC_TimeStructure.Hours = hour;
    RTC_TimeStructure.Minutes = min;
    RTC_TimeStructure.Seconds = sec;
    RTC_TimeStructure.TimeFormat = ampm;
    RTC_TimeStructure.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    RTC_TimeStructure.StoreOperation = RTC_STOREOPERATION_RESET;
    return HAL_RTC_SetTime(&hrtc, &RTC_TimeStructure, RTC_FORMAT_BIN);
}


/*
	RTC日期设置
	year,month,date:年(0~99),月(1~12),日(0~31)
	week:星期(1~7,0,非法!)
	返回值:SUCEE(1),成功
       ERROR(0),进入初始化模式失败
*/
HAL_StatusTypeDef RTC_Set_Date(uint8_t year, uint8_t month, uint8_t date, uint8_t week)
{
    RTC_DateTypeDef RTC_DateStructure;
    RTC_DateStructure.Date = date;
    RTC_DateStructure.Month = month;
    RTC_DateStructure.WeekDay = week;
    RTC_DateStructure.Year = year;
    return HAL_RTC_SetDate(&hrtc, &RTC_DateStructure, RTC_FORMAT_BIN);
}
/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
