#include "pid.h"

extern int Target_Speed;
extern int16 Distance_all;
extern int Cirque_Speed;
extern uint16 Target_Pwmin;

//平均速度环
int Car_speed=0;		
float P_Sp=0.4;       
float D_Sp=6;  
float err_car=0;		
float err_car_last=0;	
int Car_pwm=0;          
//角速度环
float P_Gy=3;       
float I_Gy=0.2;  
float err_gyro=0;    	
float err_last_gyro=0;  
float Now_gyro=0;       
int Car_pwm_gyro=0;
//方向环
float err_dir=0;       
float err_last_dir=0;   
float P_dir=23.0;
float D_dir=30.0;  
float differencial_dir=0;	


/**
 * @description: PID速度环//位置式
 */
int PID_Car_Speed(int targetspeed)
{
	static int outpwm=0;
  Car_speed=(L_speed+R_speed)*0.5;
	err_car=targetspeed-Car_speed;
	outpwm=P_Sp*err_car+D_Sp*(err_car-err_car_last);  //默认P1.2 D22//400 1.3  26.5
	if(outpwm<=-1000)outpwm=-1000;
	else if(outpwm>=1000)outpwm=1000;
	Car_pwm+=outpwm;
	err_car_last=err_car;
	//限幅输出
	if(Car_pwm>4000) Car_pwm=4000;
	if(Car_pwm<-4000)Car_pwm=-4000;
	return Car_pwm;
}


/**
 * @description: PID角速度环//增量式
 */
void PID_Car_Gyro(float Real_gyro,float target_gyro)
{
	Now_gyro=Real_gyro;//更新当前的真实角速度值
	err_gyro=target_gyro-Now_gyro;//计算PID获得增量式PID的速度更新量
	Car_pwm_gyro+=(P_Gy*(err_gyro-err_last_gyro)+I_Gy*err_gyro);//更新转向环输出占空比//默认P8 I0.8//400 9 0.5
	err_last_gyro=err_gyro;
	if(Car_pwm_gyro>10000)Car_pwm_gyro=10000;
	else if(Car_pwm_gyro<-10000)Car_pwm_gyro=-10000;
}


/**
 * @description: 方向环pid()//位置式
 */
float Speed_Control(float error)
{
	err_last_dir=err_dir;
	err_dir=error;
	differencial_dir=err_dir*P_dir+(err_dir-err_last_dir)*D_dir;//28 35//26 92
	return differencial_dir;
} 


/**
 * @description: EEPROM读取
 */
void EEPROM_Read(void)
{
//存放中间变量，用于EEPROM中的字符串数据转化为float数据，使用Str2num得到数据
char middle_sample_num[5]={0,0,0,0,0};
//EEPROM中数据写入程序
iap_read_bytes(0x0B0,middle_sample_num,4);//Target_Pwmin
Target_Pwmin=Str2num(middle_sample_num);

iap_read_bytes(0x000,middle_sample_num,4);//Speed
Target_Speed=Str2num(middle_sample_num);

iap_read_bytes(0x005,middle_sample_num,4);//P_dir
P_dir=Str2num(middle_sample_num);
	
iap_read_bytes(0x00A,middle_sample_num,4);//D_dir
D_dir=Str2num(middle_sample_num);

iap_read_bytes(0x030,middle_sample_num,4);//Distance_all
Distance_all=Str2num(middle_sample_num);

iap_read_bytes(0x010,middle_sample_num,4);//P_Sp
P_Sp=Str2num(middle_sample_num);
	
iap_read_bytes(0x015,middle_sample_num,4);//D_Sp
D_Sp=Str2num(middle_sample_num);
	
iap_read_bytes(0x020,middle_sample_num,4);//P_Gy
P_Gy=Str2num(middle_sample_num);
	
iap_read_bytes(0x025,middle_sample_num,4);//I_Gy
I_Gy=Str2num(middle_sample_num);

iap_read_bytes(0x055,middle_sample_num,4);//Cirque_Speed
Cirque_Speed=Str2num(middle_sample_num);

iap_read_bytes(0x065,middle_sample_num,4);//Cirque_GYRO
Cirque_GYRO=Str2num(middle_sample_num);

iap_read_bytes(0x075,middle_sample_num,4);//Cirque_ROAD
Cirque_ROAD=Str2num(middle_sample_num);


//圆环数据
iap_read_bytes(0x050,middle_sample_num,4);//I_Gy
Cirque[0]=Str2num(middle_sample_num);

iap_read_bytes(0x060,middle_sample_num,4);//I_Gy
Cirque[1]=Str2num(middle_sample_num);

iap_read_bytes(0x070,middle_sample_num,4);//I_Gy
Cirque[2]=Str2num(middle_sample_num);

iap_read_bytes(0x080,middle_sample_num,4);//I_Gy
Cirque[3]=Str2num(middle_sample_num);

iap_read_bytes(0x090,middle_sample_num,4);//I_Gy
Cirque[4]=Str2num(middle_sample_num);

iap_read_bytes(0x0A0,middle_sample_num,4);//I_Gy
Cirque[5]=Str2num(middle_sample_num);
} 


/**
 * @description: EEPROM写入
 */
void Write_EEPROM()
{
	 
	 iap_erase_page(0x000);	

	Num2str(Target_Pwmin);
	iap_write_bytes(0x0B0,Arry,4);//存入EEPROM}

	Num2str(Target_Speed);
	iap_write_bytes(0x000,Arry,4);//存入EEPROM}
	
	Num2str(P_dir);
	iap_write_bytes(0x005,Arry,4);//存入EEPROM}
	
	Num2str(D_dir);
	iap_write_bytes(0x00A,Arry,4);//存入EEPROM}
	
	Num2str(Distance_all);
	iap_write_bytes(0x030,Arry,4);//存入EEPROM}
	
	Num2str(P_Sp);
	iap_write_bytes(0x010,Arry,4);//存入EEPROM}
	
	Num2str(D_Sp);
	iap_write_bytes(0x015,Arry,4);//存入EEPROM}
	
	Num2str(P_Gy);
	iap_write_bytes(0x020,Arry,4);//存入EEPROM}
	
	Num2str(I_Gy);
	iap_write_bytes(0x025,Arry,4);//存入EEPROM}
	
	Num2str(Cirque_Speed);
	iap_write_bytes(0x055,Arry,4);//存入EEPROM}
	
	Num2str(Cirque_GYRO);
	iap_write_bytes(0x065,Arry,4);//存入EEPROM}
	
	Num2str(Cirque_ROAD);
	iap_write_bytes(0x075,Arry,4);//存入EEPROM}
	
	
	//圆环数据
	Num2str(Cirque[0]);
	iap_write_bytes(0x050,Arry,4);//存入EEPROM}
	
	Num2str(Cirque[1]);
	iap_write_bytes(0x060,Arry,4);//存入EEPROM}
	
	Num2str(Cirque[2]);
	iap_write_bytes(0x070,Arry,4);//存入EEPROM}
	
	Num2str(Cirque[3]);
	iap_write_bytes(0x080,Arry,4);//存入EEPROM}
	
	Num2str(Cirque[4]);
	iap_write_bytes(0x090,Arry,4);//存入EEPROM}
	
	Num2str(Cirque[5]);
	iap_write_bytes(0x0A0,Arry,4);//存入EEPROM}
	
}
