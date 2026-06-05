#include "Cirque.h"
//变量引用
extern char MODE;
extern int Speed_flag;
extern float Target_Gyro;
extern int Target_Speed;

char Cirque_Flag=0;  //环岛标志
char Cirque_Pos=0;	 //预环岛标志
int Cirque_ROAD=90;
int Cirque_GYRO=450; //设定的角速度值
uint8 Cirque_Count=0;
uint8 Cirque[6]={1,1,1,1,1,1};



 /**
 * @description:圆环编码器积分启动
 */	
void Encoder_start(void)
{
Encoder_flag=1;
Encoder_count=0;
Small_Bit=0;		//低位编码器积分
Middle_Bit=0;		//中位编码器积分
Big_Bit=0;			//高位编码器积分
}


 /**
 * @description:圆环编码器积分关闭
 */	
void Encoder_end(void)
{
Encoder_flag=0;
Encoder_count=0;
Small_Bit=0;
Middle_Bit=0;
Big_Bit=0;
}


 /**
 * @description:圆环处理
 */	
char Cirque_Design(void)
{
//中电感判断，编码器开始积分
if(Cirque_Flag==Cirque_NO&&ADC_GYH[2]>=70) //这里的参数“70”后期需要进行调试
{		
ICM_START();	
Cirque_Flag=Cirque_BEGIN;
MODE=CIRQUE_BEGIN;
Encoder_start();
Cirque_Pos=1;	
actual_angle_z=0;
}


//环岛1
if(Cirque_Flag==Cirque_BEGIN&&Cirque_Pos==1)
{
if(Middle_Bit*10000+Small_Bit>=Cirque_ROAD*100)
{
Cirque_Pos=2;//电感走到切点处
Encoder_end();	
Cirque_Count++;
}}


//环岛2，车走到切点，开始差速圆环
else if(Cirque_Flag==Cirque_BEGIN&&Cirque_Pos==2)
{
Cirque_Pos=3;
//期望角速度
if(Cirque[Cirque_Count-1]==1)
{Target_Gyro=(Cirque_GYRO);}
else if(Cirque[Cirque_Count-1]==0)
{Target_Gyro=-(Cirque_GYRO);}
MODE=CIRQUE_RUN; 
}


//环岛3，出环临界点
else if(Cirque_Flag==Cirque_BEGIN&&Cirque_Pos==3)
{ 
//到达出环临界值角度积分
if(Cirque[Cirque_Count-1]==1&&actual_angle_z>=345)
{
//编码器开始积分	
Encoder_start();
Cirque_Flag=Cirque_END;	
MODE=CIRQUE_RUN;
Target_Gyro=0;
Cirque_Pos=4;
}
else if(Cirque[Cirque_Count-1]==0&&actual_angle_z<=-345)
{	
Encoder_start();
Cirque_Flag=Cirque_END;	
MODE=CIRQUE_RUN;
Target_Gyro=0;
Cirque_Pos=4;
}}


//环岛4,到圆环与直道切点，向前走一段距离，离开环岛
else if(Cirque_Flag==Cirque_END&&Cirque_Pos==4)
{
if((Middle_Bit*10000+Small_Bit>=15000))
{
Cirque_Flag=Cirque_NO;
Cirque_Pos=0;
MODE=NORMAL_RUN;
ICM_END();
Encoder_end();	
}}
return 0;
}





