#include "Motor.h"

//变量引用
extern char MODE;
extern int Speed_flag;


//函数变量
long Distance=0;		//距离积分
int16 Star_check=0;		//启动标志
uint16 V_check=0;		//电压标志
int16 L_speed=0;		//左轮速度
int16 R_speed=0;		//右轮速度
char Encoder_flag=0;	//编码器计数标志
float Encoder_count=0;	//编码器计数
#include <STC32G.H>
int Small_Bit=0;		//低位编码器积分
int Middle_Bit=0;		//中位编码器积分
int Big_Bit=0;			//高位编码器积分
/**
 * @description: PWM初始化
 */	
void My_PWM_Init(uint16 freq,uint16 duty)
{
pwm_init(PWMA_CH4N_P33,freq,duty);
pwm_init(PWMA_CH2N_P13,freq,duty);
gpio_mode(P1_7, GPO_PP);	//设置为推挽输出
gpio_mode(P5_0, GPO_PP);	//设置为推挽输出
}

/**
 * @description: 负压PWM初始化
 */	
void My_PWM_Fuya_Init(uint16 freq,uint16 duty)
{
gpio_mode(P24, GPO_PP);	//设置为推挽输出
pwm_init(PWMA_CH3P_P24,freq,duty);
}

/**
 * @description: 赋值给左轮PWM占空比
 */
void PWM_SetCompareL(int dutyL)
{
if(dutyL>0)					//左轮正转         先统一方向控制引脚为0时，电机正转
{P17=0;
pwm_duty(PWMA_CH2N_P13,dutyL);}
else if(dutyL<0)			//左轮反转
{P17=1;
pwm_duty(PWMA_CH2N_P13,-dutyL);}
else		 				//左轮停转
{P17=0;
pwm_duty(PWMA_CH2N_P13,0);}
}

/**
 * @description: 赋值给右轮PWM占空比
 */
void PWM_SetCompareR(int dutyR)
{
if(dutyR>0)					//右轮正转
{P50=1;
pwm_duty(PWMA_CH4N_P33,dutyR);}
else if(dutyR<0)			//右轮反转
{P50=0;
pwm_duty(PWMA_CH4N_P33,-dutyR);}
else						//右轮停转
{P50=1;
pwm_duty(PWMA_CH4N_P33,0);}
}

/**
 * @description: 编码器初始化
 */
void Encoder_Init(void)
{
ctimer_count_init(CTIM3_P04);	
ctimer_count_init(CTIM0_P34);
}

/**
 * @description: 编码器采集
 */
void Encoder_get_value(void)
{
if(P47==0)
{L_speed=ctimer_count_read(CTIM3_P04);}
else if(P47==1)
{L_speed=ctimer_count_read(CTIM3_P04)*(-1);}
if(P53==1)
{R_speed=ctimer_count_read(CTIM0_P34);}
else if(P53==0)
{R_speed=ctimer_count_read(CTIM0_P34)*(-1);}
	
ctimer_count_clean(CTIM0_P34);
ctimer_count_clean(CTIM3_P04);


//高中低位编码器积分	
if(Encoder_flag==1)
{
Encoder_count+=(L_speed+R_speed)/2;
Small_Bit+=(L_speed+R_speed)/2;	
if(Small_Bit>=10000)
{
Small_Bit-=10000;
Middle_Bit+=1;
if(Middle_Bit>=1000)
{Middle_Bit-=Middle_Bit;
Big_Bit+=1;}
}
else if(Small_Bit<0)
{
if(Middle_Bit>0&&Big_Bit>0)
{Small_Bit=10000+Small_Bit;
Middle_Bit-=1;}
else 
{Small_Bit=0;
Middle_Bit=0;
Big_Bit=0;
}}
}
else Encoder_count+=0;
}

/**
 * @description: 电机停转标志位
 */
void Motor_stop(void)
{
MODE=NO_ONE;
Speed_flag=1;
}

/**
 * @description: 距离积分
 */
void Distance_count()
{Distance+=(R_speed+L_speed)/2;}

/**
 * @description: 启动按键检测
 */
void Key_Star()   //GO引脚
{
if (P51==0)
{
Star_check++;
}
}

/**
 * @description: 启动电压检测
 */
void V_Star()
{
ADC_BAT=adc_once(ADC_P16,ADC_12BIT);  
if(ADC_BAT/217.8>11.0)
V_check++;
}



