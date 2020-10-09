#ifndef __BSP_IRDA_H__
#define __BSP_IRDA_H__
#include "main.h"


#define IRDA_ID            0
#define IRDA_RCC_CLK_ENABLE           __HAL_RCC_GPIOB_CLK_ENABLE
#define IRDA_GPIO_PIN                 GPIO_PIN_1
#define IRDA_GPIO                     GPIOB
#define IRDA_EXTI_IRQn                EXTI1_IRQn
#define IRDA_EXTI_IRQHandler          EXTI1_IRQHandler
#define IrDa_DATA_IN()                HAL_GPIO_ReadPin(IRDA_GPIO,IRDA_GPIO_PIN)


extern uint32_t frame_data;
extern uint8_t  frame_cnt;
extern uint8_t  frame_flag;

void IrDa_Init(void);
uint8_t Get_Pulse_Time(void);
uint8_t IrDa_Process(void);

#endif  // __BSP_IRDA_H__
