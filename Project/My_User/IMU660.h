#ifndef __IMU660_H_
#define __IMU660_H_
#include "headfile.h"

#define G 9.80f
#define Const_Acc (32767.0f/(8.0f*G))
#define Const_Gyro (32767.0f/2000.0f)
#define To_ang(x) (x * (180.0f / PI)) ///<弧度转角度
#define To_rad(x) (x*(PI / 180.0f)) ///<弧度转角度
#define ICM_Ave_Num 8
#define ICM_Num 3

extern float Null_Shift_X;//水平方向的零飘
extern float Null_Shift_Y;//垂直方向的零漂
extern float Null_Shift_Z;//Z方向的零漂
extern float actual_angle_x;//解算得到的当前x角度
extern float actual_angle_y;//解算得到的当前y角度
extern float actual_angle_z;//解算得到的当前z角度
extern char IMU_START;//是否开始解算角度
void IMU_Init(void);
void ICM_START(void);
void ICM_END(void);
#endif
