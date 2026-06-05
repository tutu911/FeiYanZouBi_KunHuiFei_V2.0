#include "Menu.h"

uint8 KeyNum = 0;
uint8 cursor=0;//一级菜单光标
uint8 cursorplus=0;//二级菜单光标
uint8 menu_bottom_flag=0;
uint8 menu_flag=0;
int16 ADC_BAT=0;
uint16 Key_Adc=0;   //五向按键ADC读取值
extern float Weight;
extern float P_dir;       
extern float D_dir;
extern float P_Sp;       
extern float D_Sp;
extern float P_Gy;       
extern float I_Gy;  
extern int Target_Speed;
extern int Cirque_Speed;
extern int16 Distance_all;
extern char MODE;


static KeyState key_state = KEY_STATE_RELEASED;
static int stable_cnt = 0;
static KeyCode last_valid_key = KEY_NONE;
static KeyCode raw = KEY_NONE;
/**
 * @description:电感数值显示
 */
void ADC_Show(void)			
{
ips114_showstr(0,0,"Left1:");
ips114_showstr(0,1,"Left2:");
ips114_showstr(0,2,"Middle:");
ips114_showstr(0,3,"Right2:");
ips114_showstr(0,4,"Right1:");
ips114_showstr(0,6,"Error:");
ips114_showstr(0,7,"gyro_z:");	
//滤波后电感数值	
ips114_showuint16(60,0,ADC_Get[0]);					
ips114_showuint16(60,1,ADC_Get[1]);
ips114_showuint16(60,2,ADC_Get[2]);
ips114_showuint16(60,3,ADC_Get[3]);
ips114_showuint16(60,4,ADC_Get[4]);
//归一化后电感数值
ips114_showuint16(120,0,ADC_GYH[0]);						
ips114_showuint16(120,1,ADC_GYH[1]);
ips114_showuint16(120,2,ADC_GYH[2]);
ips114_showuint16(120,3,ADC_GYH[3]);
ips114_showuint16(120,4,ADC_GYH[4]);
//差比和数值误差		
ips114_showfloat(60,6,Weight,3,1);
ips114_showint16(120,6,actual_angle_z);
//角速度
ips114_showint16(60,7,imu660ra_gyro_z);
}


/**
 * @description:当前模式显示
 */
void MODE_Show(void)			
{
if(MODE==RUN_READY)
{ips114_showstr(0,6,"MODE:RUN_READY");}
else if(MODE==NORMAL_RUN)
{ips114_showstr(0,6,"MODE:NORMAL_RUN");}
else if(MODE==GYRO_TEST)
{ips114_showstr(0,6,"MODE:GYRO_TEST");}
else if(MODE==SPEED_TEST)
{ips114_showstr(0,6,"MODE:SPEED_TEST");}
else if(MODE==MOTOR_TEST)
{ips114_showstr(0,6,"MODE:MOTOR_TEST");}
}


/**
 * @description:按键扫描函数
 */
uint8 Key_Scan(void)			
{
Key_Adc=adc_once(ADC_P10, ADC_12BIT);
    // 1. 根据 ADC 值识别当前物理按键（原始码）
    if (Key_Adc >= 1400 && Key_Adc <= 1800) raw = KEY_ADD;
    else if (Key_Adc >= 500 && Key_Adc <= 900) raw = KEY_REDUCE;
	  else if (Key_Adc >= 3000 && Key_Adc <= 3300) raw = KEY_ENTER;
    else if (Key_Adc >= 1900 && Key_Adc <= 2200) raw = KEY_BACK;
    else if (Key_Adc >= 50 && Key_Adc <= 400) raw = KEY_DOWN;
    else raw = KEY_NONE;
    // 2. 连续稳定去抖（状态机）
    if (raw == last_valid_key) 
		{
        if (stable_cnt < 3) 
				{  //未达到连续3次按键有效状态相同
            stable_cnt++;
        }
        if (stable_cnt == 3) 
				{
            // 达到稳定次数，判断状态变化
            if (raw != KEY_NONE && key_state == KEY_STATE_RELEASED) 
						{
                // 有效按下事件（下降沿）
                key_state = KEY_STATE_PRESSED;
                return raw;          // 返回该按键码作为一次触发
            } 
						else if (raw == KEY_NONE && key_state == KEY_STATE_PRESSED) 
						{
                // 释放事件（上升沿）—— 不返回按键，但更新状态
                key_state = KEY_STATE_RELEASED;
            }
        }
    } 
		else 
		{
        // 当前采样与上次稳定值不同，重置计数器
        last_valid_key = raw;
        stable_cnt = 0;
        // 注意：不改变 key_state，只有连续稳定后才更新状态
    }
    return KEY_NONE;  // 未产生有效按下事件
}


/**
 * @description:第一级菜单
 */
void FirstMenu_ips114(void)	
{
//获取按键状态
KeyNum=Key_Scan();
//光标运动按键	
if(KeyNum==2)										
{
cursor++;
cursor=cursor%5;
ips114_clear_part(BLACK,0,0,24,81);
}
//二级菜单选择
if(KeyNum==5)										
{
if(cursor==0)       menu_flag=1;
else if(cursor==1)  menu_flag=2;
else if(cursor==2)  menu_flag=3;
else if(cursor==3)  menu_flag=4;
else if(cursor==4)  menu_flag=5;
else menu_flag=0;
}
//一级菜单显示
if(menu_flag==0)
{
ips114_showstr(0,cursor,"->");
ips114_showstr(25,0,"ADC_Value");
ips114_showstr(25,1,"Direction PID");
ips114_showstr(25,2,"SG PID");
ips114_showstr(25,3,"CIRQUE CHOOSE");
ips114_showstr(25,4,"Speed Data");
	
//模式显示第6行	
MODE_Show();	
//电压显示
ADC_BAT=adc_once(ADC_P16,ADC_12BIT);
ips114_showstr(0,7,"Bat_V:");
ips114_showint16(60,7,ADC_BAT);
ips114_showfloat(120,7,ADC_BAT/217.8,2,2);
	
//ips114_showstr(0,5,"i_data");
//ips114_showuint16(60,5,Key_Adc);
ips114_showstr(0,5,"Star_check");
ips114_showuint16(85,5,Star_check);

}
}


/**
 * @description:第二级菜单
 */
void SecondMenu_ips114(void)
{

////菜单1显示：ADC
if(menu_flag==1)							
{
ips114_clear(BLACK);
while(1)
{
ADC_Show();			
KeyNum=Key_Scan();
//跳出while循环，返回一级菜单
if(KeyNum==4) break;
}
ips114_clear(BLACK);
menu_flag=0;//菜单标志位清零
}


////菜单2显示:方向环PD和速度调节
else if(menu_flag==2)
{
ips114_clear(BLACK);
while(1)
{
ips114_showchar(0,cursorplus,'*');			
ips114_showstr(10,0,"Direct P:");
ips114_showstr(10,1,"Direct D:");
ips114_showstr(10,2,"Speed:");	
ips114_showstr(10,3,"Distance:");	
ips114_showstr(10,4,"Cirque_ROAD:");	
ips114_showfloat(85,0,P_dir,2,2);
ips114_showfloat(85,1,D_dir,3,2);	
ips114_showint16(85,2,Target_Speed);	
ips114_showint16(85,3,Distance_all);
ips114_showint16(105,4,Cirque_ROAD);
KeyNum=Key_Scan();
if(KeyNum==2)
{
cursorplus+=16;
cursorplus%=80;
ips114_clear_part(BLACK,0,0,10,135);
}
else if(KeyNum==1)
{
if(cursorplus==0)  		 P_dir+=0.1;
else if(cursorplus==16)  D_dir+=10;
else if(cursorplus==32)  Target_Speed+=5;
else if(cursorplus==48)  Distance_all+=5;
else if(cursorplus==64)  Cirque_ROAD+=5;
}
else if(KeyNum==3)
{
if(cursorplus==0)  		 P_dir-=0.1;
else if(cursorplus==16)  D_dir-=10;
else if(cursorplus==32)  Target_Speed-=5;
else if(cursorplus==48)  Distance_all-=5;
else if(cursorplus==64)  Cirque_ROAD-=5;
}
//保存数据，跳出while循环，返回一级菜单
else if(KeyNum==4)
{
Write_EEPROM();
break;	
}
}
ips114_clear(BLACK);
cursorplus=0;
menu_flag=0;//菜单标志位清零
}


////菜单3显示:速度环角速度环调节
else if(menu_flag==3)								
{
ips114_clear(BLACK);
while(1)
{
ips114_showchar(0,cursorplus,'*');			
ips114_showstr(10,0,"P_Sp:");
ips114_showstr(10,1,"D_Sp:");
ips114_showstr(10,2,"P_Gy:");
ips114_showstr(10,3,"I_Gy:");
ips114_showfloat(65,0,P_Sp,2,2);
ips114_showfloat(65,1,D_Sp,2,2);
ips114_showfloat(65,2,P_Gy,2,2);
ips114_showfloat(65,3,I_Gy,2,2);
KeyNum=Key_Scan();	
if(KeyNum==2)
{
cursorplus+=16;
cursorplus%=64;
ips114_clear_part(BLACK,0,0,10,100);
}
else if(KeyNum==1)
{
if(cursorplus==0)  		 P_Sp+=0.01;
else if(cursorplus==16)  D_Sp+=0.01;
else if(cursorplus==32)  P_Gy+=0.1;
else if(cursorplus==48)  I_Gy+=0.01;
}
else if(KeyNum==3)
{
if(cursorplus==0)  		 P_Sp-=0.01;
else if(cursorplus==16)  D_Sp-=0.01;
else if(cursorplus==32)  P_Gy-=0.1;
else if(cursorplus==48)  I_Gy-=0.01;
}
//保存数据，跳出while循环，返回一级菜单
else if(KeyNum==4)
{
Write_EEPROM();
break;	
}
}
ips114_clear(BLACK);
cursorplus=0;
menu_flag=0;//菜单标志位清零
}


////菜单4显示:圆环选择调节
else if(menu_flag==4)								
{
ips114_clear(BLACK);
while(1)
{
ips114_showchar(0,cursorplus,'*');			
ips114_showstr(10,0,"Cirque1:");
ips114_showstr(10,1,"Cirque2:");
ips114_showstr(10,2,"Cirque3:");
ips114_showstr(10,3,"Cirque4:");
ips114_showstr(10,4,"Cirque5:");
ips114_showstr(10,5,"Cirque6:");
ips114_showstr(10,6,"CSpeed:");
ips114_showstr(10,7,"CGYRO:");
ips114_showuint16(65,0,Cirque[0]);						
ips114_showuint16(65,1,Cirque[1]);
ips114_showuint16(65,2,Cirque[2]);
ips114_showuint16(65,3,Cirque[3]);
ips114_showuint16(65,4,Cirque[4]);
ips114_showuint16(65,5,Cirque[5]);
ips114_showuint16(80,6,Cirque_Speed);
ips114_showuint16(65,7,Cirque_GYRO);
KeyNum=Key_Scan();
	
if(KeyNum==2)
{
cursorplus+=16;
cursorplus%=128;
ips114_clear_part(BLACK,0,0,10,135);
}
else if(KeyNum==1)
{
if(cursorplus==0)  		 Cirque[0]+=1;
else if(cursorplus==16)  Cirque[1]+=1;
else if(cursorplus==32)  Cirque[2]+=1;
else if(cursorplus==48)  Cirque[3]+=1;
else if(cursorplus==64)  Cirque[4]+=1;
else if(cursorplus==80)  Cirque[5]+=1;
else if(cursorplus==96)  Cirque_Speed+=10;
else if(cursorplus==112) Cirque_GYRO+=10;
}
else if(KeyNum==3)
{
if(cursorplus==0)  		 Cirque[0]-=1;
else if(cursorplus==16)  Cirque[1]-=1;
else if(cursorplus==32)  Cirque[2]-=1;
else if(cursorplus==48)  Cirque[3]-=1;
else if(cursorplus==64)  Cirque[4]-=1;
else if(cursorplus==80)  Cirque[5]-=1;
else if(cursorplus==96)  Cirque_Speed-=10;
else if(cursorplus==112) Cirque_GYRO-=10;
}
//保存数据，跳出while循环，返回一级菜单
else if(KeyNum==4)
{
Write_EEPROM();
break;	
}}
ips114_clear(BLACK);
cursorplus=0;
menu_flag=0;//菜单标志位清零
}


////菜单5显示:数据查看
else if(menu_flag==5)								
{
ips114_clear(BLACK);
while(1)
{		
ips114_showstr(10,0,"Speed");
ips114_showstr(100,0,"P");
ips114_showstr(180,0,"D");
	
ips114_showuint16(0,1,400);
ips114_showfloat(80,1,15.2,2,2);
ips114_showuint16(160,1,100);
	
ips114_showuint16(0,2,450);
ips114_showfloat(80,2,16.0,2,2);
ips114_showuint16(160,2,120);
	
ips114_showuint16(0,3,480);
ips114_showfloat(80,3,18.2,2,2);
ips114_showuint16(160,3,160);
	
ips114_showuint16(0,4,500);
ips114_showfloat(80,4,19.0,2,2);
ips114_showuint16(160,4,160);
	
KeyNum=Key_Scan();
//跳出while循环，返回一级菜单
if(KeyNum==4)
{break;	}
}
ips114_clear(BLACK);
menu_flag=0;//菜单标志位清零
}
}


/**
 * @description:菜单动画封面
 */
void Menu_Init(void)
{
//uint8 i=0,j=0;
//if(menu_bottom_flag==0)
//{
//ips114_showstr(75,1,"WELCOME TO");
//ips114_showstr(105,3,"WZU");
//ips114_showstr(75,5," 2025 CAR");

//if(Star_check>80)
//{
//for(i=0;i<50;i++)
//{ips114_drawpoint(90+i,40,BLUE);
//delay_ms(1);}
//delay_ms(10);

//for(i=0;i<30;i++)
//{ips114_drawpoint(140,40+i,BLUE);
//delay_ms(1);}
//delay_ms(10);

//for(i=0;i<50;i++)
//{ips114_drawpoint(140-i,70,BLUE);
//delay_ms(1);}
//delay_ms(10);

//for(i=0;i<30;i++)
//{ips114_drawpoint(90,70-i,BLUE);
//delay_ms(1);}
//delay_ms(10);

//ips114_clear(BLACK);
//menu_bottom_flag=1;
//}}

//else if(menu_bottom_flag)
//{
FirstMenu_ips114();
SecondMenu_ips114();
//}
}