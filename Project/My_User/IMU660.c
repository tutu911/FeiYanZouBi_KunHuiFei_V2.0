#include "Imu660.h"

extern char READY;

float Null_Shift_X;//水平方向的零飘
float Null_Shift_Y;//垂直方向的零漂
float Null_Shift_Z;//Z方向的零漂
float actual_angle_x;//解算得到的当前x角度
float actual_angle_y;//解算得到的当前y角度
float actual_angle_z=0;			//解算得到的当前z角度
char IMU_START=0;				//是否开始解算角度

void IMU_Init(void)
{		
Null_Shift_Z=-0.323117;		 //零漂计算值
imu660ra_init();// 陀螺仪初始化
READY=1;
while(READY==1);
}


void ICM_START(void)
{	
IMU_START=1;
actual_angle_z=0;
}

void ICM_END(void)
{		
IMU_START=0;
actual_angle_z=0;
}
