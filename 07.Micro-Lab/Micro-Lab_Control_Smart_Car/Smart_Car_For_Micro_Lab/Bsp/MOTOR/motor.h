#ifndef __MOTOR_H
#define __MOTOR_H
#include "main.h"
#include "tim.h"
#include <stdio.h>

#define CCR0_Val 0
#define CCR1_Val 125
#define CCR2_Val 250
#define CCR3_Val 500
#define CCR4_Val 750
#define CCR5_Val 999


//4相控制定义
#define MOTOR_A_ON 	HAL_GPIO_WritePin(MOTOR_A_GPIO_Port, MOTOR_A_Pin, GPIO_PIN_SET);
#define MOTOR_A_OFF HAL_GPIO_WritePin(MOTOR_A_GPIO_Port, MOTOR_A_Pin, GPIO_PIN_RESET);

#define MOTOR_B_ON 	HAL_GPIO_WritePin(MOTOR_B_GPIO_Port, MOTOR_B_Pin, GPIO_PIN_SET);
#define MOTOR_B_OFF	HAL_GPIO_WritePin(MOTOR_B_GPIO_Port, MOTOR_B_Pin, GPIO_PIN_RESET);

#define MOTOR_C_ON 	HAL_GPIO_WritePin(MOTOR_C_GPIO_Port, MOTOR_C_Pin, GPIO_PIN_SET);
#define MOTOR_C_OFF	HAL_GPIO_WritePin(MOTOR_C_GPIO_Port, MOTOR_C_Pin, GPIO_PIN_RESET);

#define MOTOR_D_ON 	HAL_GPIO_WritePin(MOTOR_D_GPIO_Port, MOTOR_D_Pin, GPIO_PIN_SET);
#define MOTOR_D_OFF	HAL_GPIO_WritePin(MOTOR_D_GPIO_Port, MOTOR_D_Pin, GPIO_PIN_RESET);


extern int direction  ;
extern uint16_t Motor_Pulse ;


//前进
void car_go(void);
void car_back(void);
void car_left(void);
void car_right(void);
void car_aim(void);
void car_shot(void);
void car_stop(void);

//电机8节拍控制
void MOTOR_CONTROLD(uint8_t step, uint8_t direction);
void CLOSE_MOTOR(void);
void setServoAngle(int servoAngle);//舵机控制
#endif //__MOTOR_H

