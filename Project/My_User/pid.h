#ifndef __PID_H
#define __PID_H

#include "headfile.h"

void Write_EEPROM();
void EEPROM_Read(void);
float Speed_Control(float error);
int PID_Car_Speed(int targetspeed);
void PID_Car_Gyro(float Real_gyro,float target_gyro);

#endif

