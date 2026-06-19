///*********************************************************************************************************************
// * COPYRIGHT NOTICE
// * Copyright (c) 2020,逐飞科技
// * All rights reserved.
// * 技术讨论QQ群：一群：179029047(已满)  二群：244861897(已满)  三群：824575535
// *
// * 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
// * 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
// *
// * @file       		isr
// * @company	   		成都逐飞科技有限公司
// * @author     		逐飞科技(QQ790875685)
// * @version    		查看doc内version文件 版本说明
// * @Software 			MDK FOR C251 V5.60
// * @Target core		STC32G12K128
// * @Taobao   			https://seekfree.taobao.com/
// * @date       		2020-4-14
// ********************************************************************************************************************/
#include "headfile.h"

//UART1中断
void UART1_Isr() interrupt 4
{

    uint8 res;
	static uint8 dwon_count;
    if(UART1_GET_TX_FLAG)
    {
        UART1_CLEAR_TX_FLAG;
        busy[1] = 0;
    }
    if(UART1_GET_RX_FLAG)
    {
        UART1_CLEAR_RX_FLAG;
        res = SBUF;
        //程序自动下载
        if(res == 0x7F)
        {
            if(dwon_count++ > 20)
                IAP_CONTR = 0x60;
        }
        else
        {
            dwon_count = 0;
        }
    }
}
//UART2中断
void UART2_Isr() interrupt 8
{
    if(UART2_GET_TX_FLAG)
	{
        UART2_CLEAR_TX_FLAG;
		busy[2] = 0;
	}
    if(UART2_GET_RX_FLAG)
	{
        UART2_CLEAR_RX_FLAG;
		//接收数据寄存器为：S2BUF

	}
}
//UART3中断
void UART3_Isr() interrupt 17
{
    if(UART3_GET_TX_FLAG)
	{
        UART3_CLEAR_TX_FLAG;
		busy[3] = 0;
	}
    if(UART3_GET_RX_FLAG)
	{
        UART3_CLEAR_RX_FLAG;
		//接收数据寄存器为：S3BUF
    if(wireless_type == WIRELESS_SI24R1)
        {
            wireless_uart_callback();           //无线转串口回调函数
        }
        else if(wireless_type == WIRELESS_CH9141)
        {
            bluetooth_ch9141_uart_callback();   //蓝牙转串口回调函数
        }
        else if(wireless_type == WIRELESS_CH573)
        {
            wireless_ch573_callback();          //CH573无线模块回调函数
        }
	}
}
//UART4中断
void UART4_Isr() interrupt 18
{
    if(UART4_GET_TX_FLAG)
	{
        UART4_CLEAR_TX_FLAG;
		busy[4] = 0;
	}
    if(UART4_GET_RX_FLAG)
	{
        UART4_CLEAR_RX_FLAG;
		//接收数据寄存器为：S4BUF;
	}
}

#define LED P52
void INT0_Isr() interrupt 0
{LED = 0;}
void INT1_Isr() interrupt 2
{}
void INT2_Isr() interrupt 10
{INT2_CLEAR_FLAG;}
void INT3_Isr() interrupt 11
{INT3_CLEAR_FLAG;}
void INT4_Isr() interrupt 16
{INT4_CLEAR_FLAG;}
void TM0_Isr() interrupt 1
{}
void TM1_Isr() interrupt 3
{}
void TM2_Isr() interrupt 12
{}
void TM3_Isr() interrupt 19
{TIM3_CLEAR_FLAG;}

//各标志位变量
uint8 printf_flag =0;
int Speed_flag =0;
int flag =0;
char READY=0;
int16 Distance_all=150;  //固定跑完的距离
float Gyro_now =0;
extern float Now_gyro;       


//标志位响应,初始化时无响应
char MODE=NO_ONE;
float Weight=0;
int Speed_Test=0;     //电机测试时的目标速度
int Target_Speed=600; //目标速度值
float Target_Gyro=0;  //目标角速度
uint16 Target_Pwmin=1300; //负压占空比（100HZ：1000-2000）
int16 Delay_SP=0; 
//float LIMIT_OUT=0;
uint16 duty_set=3000; //编码器测试占空比


//环岛部分参数
extern char Cirque_Pos;   
extern int Car_pwm_gyro;
int Cirque_Speed=480;
long tly_z=0;
float tly=0;

//差比和差参数
float LQ1=1;		
float LQ2=2.5;
float LQ3=0.18;

float Bat_data=0.0;
uint16 Time_OK=1;
uint8 Fuya_Start=0;
uint8 Fuya_Ok=0;  //负压稳定标志位， 0：还未稳定 || 1：已稳定，退出计数

uint8 mode_process_flag=0; //ISR signals main loop to run MODE handler
void TM4_Isr() interrupt 20
{

	///////////////////////////////////////////////////////////
	/**
	 * @description: 陀螺仪零漂校准
	 */
	if(READY==1)
	{
		static int times=0;
		imu660ra_get_gyro();
		Null_Shift_Z=Null_Shift_Z+(imu660ra_gyro_transition(imu660ra_gyro_z));
		times++;
		if(times>=200)
		{
			Null_Shift_Z/=200.0;
			READY=0;
		}
	}

	/**
	 * @description: 陀螺仪的角度解算
	 */
	if(IMU_START==1)
	{
		static float last_velocity_z=0;
		static float velocity_z=0;
		imu660ra_get_gyro();
		velocity_z=(imu660ra_gyro_transition(imu660ra_gyro_z)-Null_Shift_Z)/1000.0;
		actual_angle_z+=5*(velocity_z+last_velocity_z)/2;
		last_velocity_z=velocity_z;
	}

	// ===== Real-time data acquisition (runs every 5ms unconditionally) =====

	/**
	 * @description: 编码器读取 + 距离累计 (无门槛)
	 */
	if (MODE == GYRO_TEST || MODE == SPEED_TEST || MODE == MOTOR_TEST || MODE == TLY_Test || MODE == NORMAL_RUN)
	{
		printf_flag = 1;
		Encoder_get_value();
		Distance_count();
	}

	Key_Star();
	V_Star();

	/**
	 * @description: ADC 原始采样 (后处理在主循环)
	 */
	ADC_Sample();
	adc_process_flag = 1;

	// Signal main loop to run MODE control handler
	mode_process_flag = 1;

	TIM4_CLEAR_FLAG;
}


// ================================================================
// MODE control functions (called from main loop, triggered by mode_process_flag)
// Each function encapsulates one MODE's control logic for readability
// ================================================================

/**
 * @description: 正常运行模式 - 电感巡线 + PID 控制
 */
static void Mode_NormalRun(void)
{
	static uint8 t_count = 0;
	static float Gyro_set = 0;
	static int Real_Speed = 0;
	int Motor_L = 0;
	int Motor_R = 0;

	if(t_count == 2)
	{
		Weight = 25 * (LQ1*(ADC_GYH[0]-ADC_GYH[4]) + LQ2*(ADC_GYH[1]-ADC_GYH[3])) /
		         (LQ1*(ADC_GYH[0]+ADC_GYH[4]) + LQ3*fabs(ADC_GYH[1]-ADC_GYH[3]));
		if(Weight > 32)  Weight = 32;
		else if(Weight < -32) Weight = -32;
		imu660ra_get_gyro();
		Gyro_set = (imu660ra_gyro_transition(imu660ra_gyro_z) - Null_Shift_Z);
		PID_Car_Gyro(Gyro_set, Speed_Control(Weight));
		t_count = 0;
	}
	t_count++;

	// Stop condition
	if(Distance > Distance_all*10000 ||
	   (ADC_GYH[0]+ADC_GYH[1]+ADC_GYH[3]+ADC_GYH[4]) < 10 ||
	   (ADC_GYH[0]+ADC_GYH[2]+ADC_GYH[4]) < 10)
	{ Motor_stop(); }

	// Soft-start speed ramp
	if(Real_Speed < Target_Speed) Real_Speed += 10;

	if(Car_pwm_gyro >= 0)
	{
		Motor_L = PID_Car_Speed(Real_Speed) - Car_pwm_gyro;
		Motor_R = PID_Car_Speed(Real_Speed) + Car_pwm_gyro;
	}
	else
	{
		Motor_L = PID_Car_Speed(Real_Speed) - Car_pwm_gyro;
		Motor_R = PID_Car_Speed(Real_Speed) + Car_pwm_gyro;
	}

	// Duty cycle limits
	if(Motor_L > 10000)  Motor_L = 10000;
	if(Motor_R > 10000)  Motor_R = 10000;
	if(Motor_L < -10000) Motor_L = -10000;
	if(Motor_R < -10000) Motor_R = -10000;

	PWM_SetCompareL(Motor_L);
	PWM_SetCompareR(Motor_R);
}

/**
 * @description: 陀螺仪测试模式 - PID 角速度环
 */
static void Mode_GyroTest(void)
{
	static uint8 t_count = 0;
	static float Gyro_Get_Now = 0;
	int Motor_L = 0;
	int Motor_R = 0;

	imu660ra_get_gyro();
	Now_gyro = (imu660ra_gyro_transition(imu660ra_gyro_z) - Null_Shift_Z);

	if(t_count == 2)
	{
		imu660ra_get_gyro();
		Gyro_Get_Now = (imu660ra_gyro_transition(imu660ra_gyro_z) - Null_Shift_Z);
		if(Distance > 70000 && Distance < 650000)
			PID_Car_Gyro(Gyro_Get_Now, 250);
		else if(Distance < 70000)
			PID_Car_Gyro(Gyro_Get_Now, 0);
		t_count = 0;
	}
	t_count++;

	if(Speed_Test < 1000) Speed_Test += 10;

	Motor_L = PID_Car_Speed(Speed_Test) - Car_pwm_gyro;
	Motor_R = PID_Car_Speed(Speed_Test) + Car_pwm_gyro;

	if(Motor_L > 10000)  Motor_L = 10000;
	if(Motor_R > 10000)  Motor_R = 10000;
	if(Motor_L < -10000) Motor_L = -10000;
	if(Motor_R < -10000) Motor_R = -10000;

	PWM_SetCompareL(Motor_L);
	PWM_SetCompareR(Motor_R);

	if(Distance > 650000) Motor_stop();
}

/**
 * @description: 速度测试模式 - 速度 PID 环
 */
static void Mode_SpeedTest(void)
{
	static uint8 t_count = 0;
	int Motor_L = 0;
	int Motor_R = 0;

	if(t_count == 2)
	{
		imu660ra_get_gyro();
		Gyro_now = (imu660ra_gyro_transition(imu660ra_gyro_z) - Null_Shift_Z);
		PID_Car_Gyro(Gyro_now, 0);
		t_count = 0;
	}
	t_count++;

	if(Speed_Test < 1000) Speed_Test += 50;

	Motor_L = PID_Car_Speed(Speed_Test) - Car_pwm_gyro;
	Motor_R = PID_Car_Speed(Speed_Test) + Car_pwm_gyro;

	if(Motor_L > 4000)  Motor_L = 4000;
	if(Motor_R > 4000)  Motor_R = 4000;
	if(Motor_L < -4000) Motor_L = -4000;
	if(Motor_R < -4000) Motor_R = -4000;

	PWM_SetCompareL(Motor_L);
	PWM_SetCompareR(Motor_R);

	if(Distance > 60000) Motor_stop();
}

/**
 * @description: 电机测试模式 - 直接 PWM 输出
 */
static void Mode_MotorTest(void)
{
	if(Distance_Beishu > 4) Motor_stop();

	if(duty_set < 4000) duty_set += 5;

	imu660ra_get_gyro();
	Gyro_now = (imu660ra_gyro_transition(imu660ra_gyro_z) - Null_Shift_Z);

	PWM_SetCompareL(duty_set);
	PWM_SetCompareR(duty_set);
}

/**
 * @description: 陀螺仪测试模式 - 角度累计
 */
static void Mode_TlyTest(void)
{
	imu660ra_get_gyro();
	Gyro_now = (imu660ra_gyro_transition(imu660ra_gyro_z) - Null_Shift_Z);
	tly += Gyro_now * 5 / 1000;
	if(tly >= 360.0)
	{
		tly_z += 1;
		tly -= 360.0;
	}
}

/**
 * @description: 刹车模式 - 停止所有输出
 */
static void Mode_NoOne(void)
{
	PWM_SetCompareL(0);
	PWM_SetCompareR(0);
	Speed_flag = 1;
}

// ================================================================

/**
 * @description: MODE 调度器 - 由主循环调用, 每 5ms 执行一次
 *              包含 Star_check 门槛、预压延时、V_check 保护
 */
void Mode_Handler(void)
{
	if(Star_check > 10)
	{
		if(Fuya_Ok == 0) Time_OK++;
		if(Time_OK > 400)
		{
			Fuya_Ok = 1;
			ADC_BAT = adc_once(ADC_P16, ADC_12BIT);
			Bat_data = ADC_BAT / 217.8;

			if(V_check == 0)
			{
				Motor_stop();
			}
			else
			{
				if(MODE == NORMAL_RUN && Speed_flag == 0)
					Mode_NormalRun();
				else if(MODE == GYRO_TEST && Speed_flag == 0)
					Mode_GyroTest();
				else if(MODE == TLY_Test)
					Mode_TlyTest();
				else if(MODE == SPEED_TEST && Speed_flag == 0)
					Mode_SpeedTest();
				else if(MODE == MOTOR_TEST && Speed_flag == 0)
					Mode_MotorTest();
				else if(MODE == NO_ONE)
					Mode_NoOne();
			}
		}
	}
	else
	{
		// Star_check <= 10: not started, ensure motors stopped
		PWM_SetCompareL(0);
		PWM_SetCompareR(0);
	}
}


//void  INT0_Isr()  interrupt 0;
//void  TM0_Isr()   interrupt 1;
//void  INT1_Isr()  interrupt 2;
//void  TM1_Isr()   interrupt 3;
//void  UART1_Isr() interrupt 4;
//void  ADC_Isr()   interrupt 5;
//void  LVD_Isr()   interrupt 6;
//void  PCA_Isr()   interrupt 7;
//void  UART2_Isr() interrupt 8;
//void  SPI_Isr()   interrupt 9;
//void  INT2_Isr()  interrupt 10;
//void  INT3_Isr()  interrupt 11;
//void  TM2_Isr()   interrupt 12;
//void  INT4_Isr()  interrupt 16;
//void  UART3_Isr() interrupt 17;
//void  UART4_Isr() interrupt 18;
//void  TM3_Isr()   interrupt 19;
//void  TM4_Isr()   interrupt 20;
//void  CMP_Isr()   interrupt 21;
//void  I2C_Isr()   interrupt 24;
//void  USB_Isr()   interrupt 25;
//void  PWM1_Isr()  interrupt 26;
//void  PWM2_Isr()  interrupt 27;