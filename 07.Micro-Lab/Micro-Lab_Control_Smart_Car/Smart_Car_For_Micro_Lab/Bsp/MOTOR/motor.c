#include "motor.h"


int direction = 0 ;
uint16_t Motor_Pulse = 0 ;
//电机脉冲计算
int Motor_Angle_Cal(int Rotation_Angle);
//关闭电机
void CLOSE_MOTOR(void);
static void Motor_PWM_SetValue(TIM_HandleTypeDef *htim, uint32_t Channe, unsigned short value);

void car_go(void)
{
    Motor_PWM_SetValue(&htim1, TIM_CHANNEL_1, CCR3_Val); //1
    Motor_PWM_SetValue(&htim1, TIM_CHANNEL_2, CCR0_Val); //0
    Motor_PWM_SetValue(&htim1, TIM_CHANNEL_3, CCR3_Val); //1
    Motor_PWM_SetValue(&htim1, TIM_CHANNEL_4, CCR0_Val); //0
}

void car_back(void)
{
    Motor_PWM_SetValue(&htim1, TIM_CHANNEL_1, CCR0_Val); //0
    Motor_PWM_SetValue(&htim1, TIM_CHANNEL_2, CCR3_Val); //1
    Motor_PWM_SetValue(&htim1, TIM_CHANNEL_3, CCR0_Val); //0
    Motor_PWM_SetValue(&htim1, TIM_CHANNEL_4, CCR3_Val); //1
}

void car_left(void)
{
    Motor_PWM_SetValue(&htim1, TIM_CHANNEL_1, CCR0_Val); //0		==> 控制左前轮后
    Motor_PWM_SetValue(&htim1, TIM_CHANNEL_2, CCR0_Val); //1		==> 控制左前轮前	===> 慢
    Motor_PWM_SetValue(&htim1, TIM_CHANNEL_3, CCR3_Val); //1		==> 控制右前轮前	===> 快
    Motor_PWM_SetValue(&htim1, TIM_CHANNEL_4, CCR0_Val); //0		==> 控制右前轮后
}

void car_right(void)
{
    Motor_PWM_SetValue(&htim1, TIM_CHANNEL_1, CCR3_Val); //0
    Motor_PWM_SetValue(&htim1, TIM_CHANNEL_2, CCR0_Val); //1 快
    Motor_PWM_SetValue(&htim1, TIM_CHANNEL_3, CCR0_Val); //1 慢
    Motor_PWM_SetValue(&htim1, TIM_CHANNEL_4, CCR0_Val); //0
}

void car_aim(void)
{
    int Angle_adjustment = 0 ;
    static int count_angle = 0;
    //设置按键一次步进的角度为45度
    count_angle = 45 ;
    //角度微调度数设置为1
    Angle_adjustment = 1 ;
    direction = 0 ;
    Motor_Angle_Cal(count_angle);
}

void car_shot(void)
{
    HAL_GPIO_WritePin(take_aim_GPIO_Port, take_aim_Pin, 1);
    HAL_GPIO_WritePin(SHOT_ALARM_GPIO_Port, SHOT_ALARM_Pin, 1);
}

void car_stop(void)
{
    Motor_PWM_SetValue(&htim1, TIM_CHANNEL_1, CCR0_Val);
    Motor_PWM_SetValue(&htim1, TIM_CHANNEL_2, CCR0_Val);
    Motor_PWM_SetValue(&htim1, TIM_CHANNEL_3, CCR0_Val);
    Motor_PWM_SetValue(&htim1, TIM_CHANNEL_4, CCR0_Val);
    HAL_GPIO_WritePin(take_aim_GPIO_Port, take_aim_Pin, 0);
    HAL_GPIO_WritePin(SHOT_ALARM_GPIO_Port, SHOT_ALARM_Pin, 0);
}


/*
	Rotation_Angle:旋转角度
	返回：Motor_Pulse 根据公式计算得出的脉冲数
*/
int Motor_Angle_Cal(int Rotation_Angle)
{
    Motor_Pulse = (int)((double)(Rotation_Angle / 5.625) * 64) ;
    return Motor_Pulse ;
}

static void Motor_PWM_SetValue(TIM_HandleTypeDef *htim, uint32_t Channe, unsigned short value)
{
    TIM_OC_InitTypeDef sConfigOC;

    if(value >= 1000 - 1) value = 999;

    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = value;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
    sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;

    if (HAL_TIM_PWM_ConfigChannel(htim, &sConfigOC, Channe) != HAL_OK)
    {
        Error_Handler();
    }

    if (HAL_TIM_PWM_Start(htim, Channe) != HAL_OK)
    {
        Error_Handler();
    }
}

//servoAngle:-90~90
void setServoAngle(int servoAngle)//舵机控制
{
    uint8_t positive_angle = servoAngle + 90;
    uint8_t pwm_duty = positive_angle + 60; //positive_angle*1.11+50;
    printf("pwm:%d\n", pwm_duty);
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, pwm_duty);
}

//电机控制，采用8节拍来做
//A->AB->B->BC->C->CD->D->DA
void MOTOR_CONTROLD(uint8_t step, uint8_t direction)
{
    uint8_t __step = step ;

    //判断电机的旋转方向，如果为1，则逆向旋转
    if(1 == direction)
        __step = 8 - step ;

    switch(__step)
    {
        //A
        case 0:
            MOTOR_A_ON;
            MOTOR_B_OFF;
            MOTOR_C_OFF;
            MOTOR_D_OFF;
            break ;

        //AB
        case 1:
            MOTOR_A_ON;
            MOTOR_B_ON;
            MOTOR_C_OFF;
            MOTOR_D_OFF;
            break ;

        //B
        case 2:
            MOTOR_A_OFF;
            MOTOR_B_ON;
            MOTOR_C_OFF;
            MOTOR_D_OFF;
            break ;

        //BC
        case 3:
            MOTOR_A_OFF;
            MOTOR_B_ON;
            MOTOR_C_ON;
            MOTOR_D_OFF;
            break ;

        //C
        case 4:
            MOTOR_A_OFF;
            MOTOR_B_OFF;
            MOTOR_C_ON;
            MOTOR_D_OFF;
            break ;

        //CD
        case 5:
            MOTOR_A_OFF;
            MOTOR_B_OFF;
            MOTOR_C_ON;
            MOTOR_D_ON;
            break ;

        //D
        case 6:
            MOTOR_A_OFF;
            MOTOR_B_OFF;
            MOTOR_C_OFF;
            MOTOR_D_ON;

        //DA
        case 7:
            MOTOR_A_ON;
            MOTOR_B_OFF;
            MOTOR_C_OFF;
            MOTOR_D_ON;
            break ;
    }
}

//关闭电机
void CLOSE_MOTOR(void)
{
    HAL_GPIO_WritePin(GPIOA, MOTOR_A_Pin | MOTOR_B_Pin | MOTOR_C_Pin | MOTOR_D_Pin, GPIO_PIN_RESET);
}
