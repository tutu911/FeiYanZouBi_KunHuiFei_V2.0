/*********************************************************************************************************************
 * COPYRIGHT NOTICE
 * Copyright (c) 2018,逐飞科技
 * All rights reserved.
 *
 * LSM6DSRTR 驱动 (STMicroelectronics)
 * 原 IMU660RA 头文件, 已适配 LSM6DSRTR 芯片
 *
 * 接线定义:
 *                   ------------------------------------
 *                   模块管脚            单片机引脚
 *                   //  软件 SPI 引脚
 *                   SCL/SPC           IMU660RA_SPC_PIN  (P40)
 *                   SDA/DSI           IMU660RA_SDI_PIN  (P41)
 *                   SA0/SDO           IMU660RA_SDO_PIN  (P42)
 *                   CS                IMU660RA_CS_PIN   (P43)
 *                   VCC               3.3V电源
 *                   GND               电源地
 *                   其余引脚悬空
 *                   ------------------------------------
********************************************************************************************************************/

#ifndef _SEEKFREE_IMU660RA_h_
#define _SEEKFREE_IMU660RA_h_

#include "common.h"
#include "board.h"


#define IMU660RA_USE_SOFT_IIC       	(0)         // 默认使用软件 SPI 方式驱动

#if IMU660RA_USE_SOFT_IIC
//=====================================================软件 IIC 驱动====================================================
	#define IMU660RA_SCL_PIN            (P40)     	// 软件 IIC SCL 引脚 连接 LSM6DSR 的 SCL 引脚
	#define IMU660RA_SDA_PIN            (P41)      	// 软件 IIC SDA 引脚 连接 LSM6DSR 的 SDA 引脚
	#define IMU660RA_SOFT_IIC_DELAY     (0 )   		// 软件 IIC 的时钟延时周期 数值越大 IIC 通信速率越快
//=====================================================软件 IIC 驱动====================================================
#else
//=====================================================软件 SPI 驱动====================================================
	#define IMU660RA_SPC_PIN            (P40)      	// 软件 SPI SCK 引脚
	#define IMU660RA_SDI_PIN            (P41)      	// 软件 SPI MOSI 引脚
	#define IMU660RA_SDO_PIN            (P42)      	// 软件 SPI MISO 引脚
	#define IMU660RA_CS_PIN             (P43)      	// 软件 SPI CS 引脚
//=====================================================软件 SPI 驱动====================================================
#endif


#define IMU660RA_TIMEOUT_COUNT      (0x00FF)                                    // 自检超时计数

#define IMU660RA_DEV_ADDR           (0x6B)                                      // LSM6DSR I2C 地址 (SA0=HIGH: 0x6B)
#define IMU660RA_SPI_W              (0x00)                                      // SPI 写标志 (bit7=0)
#define IMU660RA_SPI_R              (0x80)                                      // SPI 读标志 (bit7=1)

// ---- LSM6DSRTR 寄存器地址定义 ----
#define IMU660RA_CHIP_ID            (0x0F)      // WHO_AM_I  复位值: 0x6B

#define IMU660RA_CTRL1_XL           (0x10)      // 加速度控制寄存器1 (ODR + FS)
#define IMU660RA_CTRL2_G            (0x11)      // 陀螺仪控制寄存器2 (ODR + FS)
#define IMU660RA_CTRL3_C            (0x12)      // 控制寄存器3 (BDU, IF_INC, SW_RESET等)
#define IMU660RA_CTRL4_C            (0x13)      // 控制寄存器4
#define IMU660RA_CTRL5_C            (0x14)      // 控制寄存器5
#define IMU660RA_CTRL6_C            (0x15)      // 控制寄存器6
#define IMU660RA_CTRL7_G            (0x16)      // 陀螺仪控制寄存器7
#define IMU660RA_CTRL8_XL           (0x17)      // 加速度控制寄存器8
#define IMU660RA_CTRL9_XL           (0x18)      // 加速度控制寄存器9
#define IMU660RA_CTRL10_C           (0x19)      // 控制寄存器10

#define IMU660RA_STATUS_REG         (0x1E)      // 状态寄存器 (XLDA, GDA, TDA)

// 数据输出寄存器 (自动递增读取)
#define IMU660RA_GYRO_ADDRESS       (0x22)      // OUTX_L_G 陀螺仪 X 低字节
                                                // 连续6字节: X_L, X_H, Y_L, Y_H, Z_L, Z_H

#define IMU660RA_ACC_ADDRESS        (0x28)      // OUTX_L_A 加速度 X 低字节
                                                // 连续6字节: X_L, X_H, Y_L, Y_H, Z_L, Z_H

// ---- 兼容旧宏名 ----
#define IMU660RA_ACC_CONF           IMU660RA_CTRL1_XL
#define IMU660RA_GYR_CONF           IMU660RA_CTRL2_G
#define IMU660RA_ACC_RANGE          IMU660RA_CTRL1_XL
#define IMU660RA_GYR_RANGE          IMU660RA_CTRL2_G

// ---- 量程/采样率选择 ----
// 加速度计量程选择:
// 0x00: ±2g   (16384 LSB/g)
// 0x01: ±4g   (8192  LSB/g)
// 0x02: ±8g   (4096  LSB/g)
// 0x03: ±16g  (2048  LSB/g)
#define IMU660RA_ACC_SAMPLE         (0x02)

// 陀螺仪量程选择:
// 0x00: ±2000dps  (16.4  LSB/dps)
// 0x01: ±1000dps  (32.8  LSB/dps)
// 0x02: ±500dps   (65.6  LSB/dps)
// 0x03: ±250dps   (131.2 LSB/dps)
// 0x04: ±125dps   (262.4 LSB/dps)
#define IMU660RA_GYR_SAMPLE         (0x00)


extern int16 imu660ra_gyro_x, imu660ra_gyro_y, imu660ra_gyro_z;
extern int16 imu660ra_acc_x, imu660ra_acc_y, imu660ra_acc_z;


void  imu660ra_get_acc              (void);
void  imu660ra_get_gyro             (void);
float imu660ra_acc_transition       (int16 acc_value);
float imu660ra_gyro_transition      (int16 gyro_value);
uint8 imu660ra_init                 (void);

#endif
