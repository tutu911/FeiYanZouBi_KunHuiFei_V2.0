/*********************************************************************************************************************
 * COPYRIGHT NOTICE
 * Copyright (c) 2025,温州大学 驰飞一队
 * All rights reserved.
 *
 * @file       		main
 * @author     		驰飞一队
 * @Software 		MDK FOR C251 V5.60
 * @Target core		STC32G12K128
 * @date       		2025-7-1
 ********************************************************************************************************************/
#include "headfile.h"

//变量引用
extern char MODE;	
extern uint8 printf_flag;
extern int Speed_Test; 
extern float Weight;
extern float Now_gyro;
extern float Gyro_now;
extern uint16 duty_set;
extern int Car_pwm_gyro;
//extern uint32 Distance;
void main()
{

board_init();						//初始化寄存器
ips114_init(); 						//IPS屏幕初始化
// 初始化开始
ips114_showstr(0,1,"Everything Init Start!");	

//软硬件初始化
ADC_Init1();                           //ADC采集初始化
Encoder_Init();                        //编码器初始化
My_PWM_Init(20000,0);	                 //驱动PWM初始化
My_PWM_Fuya_Init(100,0);    //负压PWM初始化
wireless_uart_init();				           //无线串口初始化
NVIC_SetPriority(UART3_IRQn,3);		     //串口3优先级最高
iap_init();	 						               //EEPROM初始化
EEPROM_Read();						             //EEPROM读取
pit_timer_ms(TIM_4,5);				         //定时器一初始化
IMU_Init();                            //陀螺仪初始化
IMU_START=1;						               //陀螺仪解算开始

// 模式选择
MODE=MOTOR_TEST;
	
//初始化成功
ips114_showstr(0,2,"Everything Is OK!");
delay_ms(50);
ips114_clear(BLACK);

while(1)
{

//数据打印
if(printf_flag==1&&MODE!=NO_ONE)
{
if(MODE==MOTOR_TEST)
{printf("%d,%d,%d,%d,%d\n",duty_set,R_speed,L_speed,Distance,Distance_Beishu);}                //电机编码器测试打印
else if(MODE==SPEED_TEST)
{printf("%d,%d,%d\n",Speed_Test,(L_speed+R_speed)/2,Car_pwm_gyro);} //速度环测试打印
else if(MODE==GYRO_TEST)
{printf("%d,%d\n",250,(int)Now_gyro);}              //角速度环测试打印
else if(MODE==NORMAL_RUN)
{printf("%f,%d\n",Weight,Car_pwm_gyro);}                            //方向环输出打印
else if(MODE==TLY_Test)
{printf("%f\n",Gyro_now);}                            //陀螺仪测试打印
printf_flag=0;          //关闭标志位，确保进入中断更新数据之后再打印
}

if(Star_check<10)         //五向按键按下的那一向：标志调试状态
{
Menu_Init();                           //菜单初始化
}
}
}
