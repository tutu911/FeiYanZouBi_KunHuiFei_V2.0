#ifndef __CIRQUE_H
#define __CIRQUE_H

#include "headfile.h"



enum Round
{
Cirque_NO=0,
Cirque_BEGIN,
Cirque_END,
};

char Cirque_Design(void);
char Ready_Run(void);
void Encoder_start(void);
void Encoder_end(void);
void Beep_Init(void);
extern uint8 Cirque[6];
extern uint8 Cirque_Count;
extern int Cirque_GYRO;
extern int Cirque_ROAD;
extern char Cirque_Flag;

#endif
