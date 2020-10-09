#include "bsp_irda.h"

uint32_t frame_data=0;    /* 一帧数据缓存 */
uint8_t  frame_cnt=0;
uint8_t  frame_flag=0;    /* 一帧数据接收完成标志 */

void IrDa_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  IRDA_RCC_CLK_ENABLE();

  /*Configure GPIO pin : PB1 */
  GPIO_InitStruct.Pin = IRDA_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(IRDA_GPIO, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(IRDA_EXTI_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(IRDA_EXTI_IRQn);
}

/**
  * 函数功能: 获取高电平的时间
  * 输入参数: 无
  * 返 回 值: 时间
  * 说    明：无
  */
uint8_t Get_Pulse_Time(void)
{
  uint8_t time = 0;
  while( IrDa_DATA_IN() )
  {
    time ++;
    HAL_Delay(2);     // 延时 20us
    if(time == 250)
      return time;   // 超时溢出   
  }
  return time;
}

/**
  * 函数功能: 获取帧数据
  * 输入参数: 无
  * 返 回 值: 帧数据
  * 说    明：帧数据有4个字节，第一个字节是遥控的ID，第二个字节是
  *           第一个字节的反码；第三个数据是遥控的真正的键值，
  *           第四个字节是第三个字节的反码
  */
uint8_t IrDa_Process(void)
{
  uint8_t first_byte, sec_byte, tir_byte, fou_byte;  
  
  first_byte = frame_data >> 24;
  sec_byte = (frame_data>>16) & 0xff;
  tir_byte = frame_data >> 8;
  fou_byte = frame_data;
  
  /* 记得清标志位 */
  frame_flag = 0;
  
  if( (first_byte==(uint8_t)~sec_byte) && (first_byte==IRDA_ID) )
  {
    if( tir_byte == (uint8_t)~fou_byte )
      return tir_byte;
  }
  
  return 0;   /* 错误返回 */
}
