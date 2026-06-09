#ifndef __PROTOCOL_H
#define __PROTOCOL_H

#include "headfile.h"

/**
 * @description: 上位机PID调参协议处理函数
 *               在主循环中调用，非阻塞读取无线串口接收FIFO，
 *               解析文本指令并更新对应的PID参数全局变量。
 * @协议格式:   KEY=VALUE\n  或  KEY:VALUE\n  或  KEY VALUE\n
 *             支持命令: P_dir, D_dir, P_Sp, D_Sp, P_Gy, I_Gy,
 *                       Target_Speed(TS), Distance(Dist),
 *                       Cirque_Speed(CS), Cirque_GYRO(CG),
 *                       Cirque_ROAD(CR), Target_Pwmin(PW)
 *                       save (保存到EEPROM)
 */
void Protocol_Process(void);

#endif
