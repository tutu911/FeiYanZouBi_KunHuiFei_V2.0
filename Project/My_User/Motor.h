#ifndef __MOTOR_H
#define __MOTOR_H

#include "headfile.h"

//PWM 引脚设置
//#define PWM_DIRL P17
//#define PWM_DIRR P50
//#define PWM_L PWMA_CH2N_P13 
//#define PWM_R PWMA_CH4N_P33

//编码器引脚设置
#define SPEEDL_DIR     P47
#define SPEEDR_DIR     P53
//#define SPEEDL_PLUSE   CTIM3_P04
//#define SPEEDR_PLUSE   CTIM0_P34

extern int16 L_speed;
extern int16 R_speed;
extern long Distance;
extern int16 Star_check;
extern uint16 V_check;
extern char Encoder_flag;
extern float Encoder_count;
extern int Small_Bit;
extern int Middle_Bit;
extern int Big_Bit;

void V_Star();
void Key_Star();
void Motor_stop(void);
void Distance_count();
void Encoder_Init(void);
void Encoder_get_value(void);
void PWM_SetCompareL(int dutyL);
void PWM_SetCompareR(int dutyR);
void My_PWM_Init(uint16 freq,uint16 duty);
void My_PWM_Fuya_Init(uint16 freq,uint16 duty);
#endif
