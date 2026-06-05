/*********************************************************************************************************************
 * COPYRIGHT NOTICE
 * Copyright (c) 2018,逐�?��?�技
 * All rights reserved.
 * 技�??�?�??�QQ群：一群：179029047(已满)  二群�??244861897
 *
 * 以下所有内容版权均属逐�?��?�技所有，�??经允许不得用于商业用途，
 * 欢迎各位使用并传�??�??程序，修改内容时必须保留逐�?��?�技的版权声明�?
 *
 * @file       		IMU660RA
 * @company	   		成都逐�?��?�技有限�??�??
 * @author     		逐�?��?�技(QQ3184284598)
 * @version    		查看doc内version文件 版本说明
 * @Software 		MDK FOR C251 V5.60
 * @Target core		STC32G12K128
 * @Taobao   		https://seekfree.taobao.com/
 * @date       		2019-04-30
 * @note
 * 接线定义�??
 *                   ------------------------------------
 *                   模块管脚            单片机�?�脚
 *                   // �??�?? SPI 引脚
 *                   SCL/SPC           查看 SEEKFREE_IMU660RA.h �?? IMU660RA_SPC_PIN 宏定�??
 *                   SDA/DSI           查看 SEEKFREE_IMU660RA.h �?? IMU660RA_SDI_PIN 宏定�??
 *                   SA0/SDO           查看 SEEKFREE_IMU660RA.h �?? IMU660RA_SDO_PIN 宏定�??
 *                   CS                查看 SEEKFREE_IMU660RA.h �?? IMU660RA_CS_PIN 宏定�??
 *                   VCC               3.3V电源
 *                   GND               电源�??
 *                   其余引脚�??�??
 *
 *                   // �??�?? IIC 引脚
 *                   SCL/SPC           查看 SEEKFREE_IMU660RA.h �?? IMU660RA_SCL_PIN 宏定�??
 *                   SDA/DSI           查看 SEEKFREE_IMU660RA.h �?? IMU660RA_SDA_PIN 宏定�??
 *                   VCC               3.3V电源
 *                   GND               电源�??
 *                   其余引脚�??�??
 *                   ------------------------------------
********************************************************************************************************************/

#include "SEEKFREE_IMU660RA.h"

#include "zf_delay.h"
#include "zf_spi.h"


#pragma warning disable = 177
#pragma warning disable = 183


int16 imu660ra_gyro_x = 0, imu660ra_gyro_y = 0, imu660ra_gyro_z = 0;            // 三轴陀螺仪数据   gyro (陀螺仪)
int16 imu660ra_acc_x = 0, imu660ra_acc_y = 0, imu660ra_acc_z = 0;               // 三轴加速度计数�?? acc  (accelerometer 加速度�??)

#if IMU660RA_USE_SOFT_IIC

#define GET_IMU660RA_SDA   		 		IMU660RA_SDA_PIN
#define IMU660RA_SCL_LOW()          	IMU660RA_SCL_PIN = 0		//IO口输出低电平
#define IMU660RA_SCL_HIGH()         	IMU660RA_SCL_PIN = 1		//IO口输出高电平
#define IMU660RA_SDA_LOW()          	IMU660RA_SDA_PIN = 0		//IO口输出低电平
#define IMU660RA_SDA_HIGH()         	IMU660RA_SDA_PIN = 1		//IO口输出高电平


#define ack 1      //主应�??
#define no_ack 0   //从应�??	

//-------------------------------------------------------------------------------------------------------------------
//  @brief      模拟IIC延时
//  @return     void
//  @since      v1.0
//  Sample usage:				如果IIC通�??失败�??以尝试�?�加j的�?
//-------------------------------------------------------------------------------------------------------------------
static void imu660ra_simiic_delay(void)
{
    uint16 xdata j=IMU660RA_SOFT_IIC_DELAY;
    while(j--);
}

//内部使用，用户无需调用
static void imu660ra_simiic_start(void)
{
    IMU660RA_SDA_HIGH();
    IMU660RA_SCL_HIGH();
    imu660ra_simiic_delay();
    IMU660RA_SDA_LOW();
    imu660ra_simiic_delay();
    IMU660RA_SCL_LOW();
}

//内部使用，用户无需调用
static void imu660ra_simiic_stop(void)
{
    IMU660RA_SDA_LOW();
    IMU660RA_SCL_LOW();
    imu660ra_simiic_delay();
    IMU660RA_SCL_HIGH();
    imu660ra_simiic_delay();
    IMU660RA_SDA_HIGH();
    imu660ra_simiic_delay();
}

//主应�??(包含ack:SDA=0和no_ack:SDA=0)
//内部使用，用户无需调用
static void imu660ra_simiic_sendack(unsigned char ack_dat)
{
    IMU660RA_SCL_LOW();
    imu660ra_simiic_delay();
    if(ack_dat) IMU660RA_SDA_LOW();
    else    	IMU660RA_SDA_HIGH();
    IMU660RA_SCL_HIGH();
    imu660ra_simiic_delay();
    IMU660RA_SCL_LOW();
    imu660ra_simiic_delay();
}


static int imu660ra_sccb_waitack(void)
{
    IMU660RA_SCL_LOW();
    imu660ra_simiic_delay();
    IMU660RA_SCL_HIGH();
    imu660ra_simiic_delay();
    if(GET_IMU660RA_SDA)           //应答为高电平，异常，通信失败
    {
        IMU660RA_SCL_LOW();
        return 0;
    }
    IMU660RA_SCL_LOW();
    imu660ra_simiic_delay();
    return 1;
}

//字节发送程�??
//发送c(�??以是数据也可�??地址)，送完后接收从应答
//不考虑从应答位
//内部使用，用户无需调用
static void imu660ra_send_ch(uint8 c)
{
    uint8 xdata i = 8;
    while(i--)
    {
        if(c & 0x80)	IMU660RA_SDA_HIGH();//SDA 输出数据
        else			IMU660RA_SDA_LOW();
        c <<= 1;
        imu660ra_simiic_delay();
        IMU660RA_SCL_HIGH();                //SCL 拉高，采集信�??
        imu660ra_simiic_delay();
        IMU660RA_SCL_LOW();                //SCL 时钟线拉�??
    }
    imu660ra_sccb_waitack();
}


//字节接收程序
//接收器件传来的数�??，�?�程序应配合|主应答函数|使用
//内部使用，用户无需调用
static uint8 imu660ra_read_ch(uint8 ack_x)
{
    uint8 xdata i;
    uint8 xdata c;
    c=0;
    IMU660RA_SCL_LOW();
    imu660ra_simiic_delay();
    IMU660RA_SDA_HIGH();
    for(i=0; i<8; i++)
    {
        imu660ra_simiic_delay();
        IMU660RA_SCL_LOW();         //�??时钟线为低，准�?�接收数�??�??
        imu660ra_simiic_delay();
        IMU660RA_SCL_HIGH();         //�??时钟线为高，使数�??线上数据有效
        imu660ra_simiic_delay();
        c<<=1;
        if(GET_IMU660RA_SDA)
        {
            c+=1;   //读数�??位，将接收的数据存c
        }
    }
    IMU660RA_SCL_LOW();
    imu660ra_simiic_delay();
    imu660ra_simiic_sendack(ack_x);
    return c;
}


//-------------------------------------------------------------------------------------------------------------------
//  @brief      模拟IIC写数�??到�?��?�寄存器函数
//  @param      dev_add			设�?�地址(低七位地址)
//  @param      reg				寄存器地址
//  @param      dat				写入的数�??
//  @return     void
//  @since      v1.0
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
static void imu660ra_simiic_write_reg(uint8 dev_add, uint8 reg, uint8 dat)
{
    imu660ra_simiic_start();
    imu660ra_send_ch( (dev_add<<1) | 0x00);   //发送器件地址加写�??
    imu660ra_send_ch( reg );   				 //发送从机寄存器地址
    imu660ra_send_ch( dat );   				 //发送需要写入的数据
    imu660ra_simiic_stop();
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      模拟IIC写数�??到�?��?�寄存器函数
//  @param      dev_add			设�?�地址(低七位地址)
//  @param      reg				寄存器地址
//  @param      dat				写入的数�??
//  @return     void
//  @since      v1.0
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
static void imu660ra_simiic_write_regs(uint8 dev_add, uint8 reg, uint8 *dat, uint32 len)
{
    uint16 xdata i = 0;
    imu660ra_simiic_start();
    imu660ra_send_ch( (dev_add<<1) | 0x00);   //发送器件地址加写�??
    imu660ra_send_ch( reg );   				 //发送从机寄存器地址
    while(len--)
    {
        imu660ra_send_ch( *dat++ );   				 //发送需要写入的数据
    }
    imu660ra_simiic_stop();
}
//-------------------------------------------------------------------------------------------------------------------
//  @brief      模拟IIC从�?��?�寄存器读取数据
//  @param      dev_add			设�?�地址(低七位地址)
//  @param      reg				寄存器地址
//  @param      type			选择通信方式是IIC  还是 SCCB
//  @return     uint8 xdata			返回寄存器的数据
//  @since      v1.0
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
static uint8 imu660ra_simiic_read_reg(uint8 dev_add, uint8 reg)
{
    uint8 xdata dat;
    imu660ra_simiic_start();
    imu660ra_send_ch( (dev_add<<1) | 0x00);  //发送器件地址加写�??
    imu660ra_send_ch( reg );   				//发送从机寄存器地址
    imu660ra_simiic_start();
    imu660ra_send_ch( (dev_add<<1) | 0x01);  //发送器件地址加�?�位
    dat = imu660ra_read_ch(no_ack);   				//读取数据
    imu660ra_simiic_stop();
    return dat;
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      模拟IIC读取多字节数�??
//  @param      dev_add			设�?�地址(低七位地址)
//  @param      reg				寄存器地址
//  @param      dat_add			数据保存的地址指针
//  @param      num				读取字节数量
//  @param      type			选择通信方式是IIC  还是 SCCB
//  @return     uint8 xdata			返回寄存器的数据
//  @since      v1.0
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
static void imu660ra_simiic_read_regs(uint8 dev_add, uint8 reg, uint8 *dat_add, uint32 num)
{
    imu660ra_simiic_start();
    imu660ra_send_ch( (dev_add<<1) | 0x00);  //发送器件地址加写�??
    imu660ra_send_ch( reg );   				//发送从机寄存器地址
    imu660ra_simiic_start();
    imu660ra_send_ch( (dev_add<<1) | 0x01);  //发送器件地址加�?�位
    while(--num)
    {
        *dat_add = imu660ra_read_ch(ack); //读取数据
        dat_add++;
    }
    *dat_add = imu660ra_read_ch(no_ack); //读取数据
    imu660ra_simiic_stop();
}

#define imu660ra_write_register(reg, dat)        (imu660ra_simiic_write_reg(IMU660RA_DEV_ADDR, (reg), (dat)))
#define imu660ra_write_registers(reg, dat, len)  (imu660ra_simiic_write_regs(IMU660RA_DEV_ADDR, (reg), (dat), (len)))
#define imu660ra_read_register(reg)              (imu660ra_simiic_read_reg(IMU660RA_DEV_ADDR, (reg)))
#define imu660ra_read_registers(reg, dat, len)   (imu660ra_simiic_read_regs(IMU660RA_DEV_ADDR, (reg), (dat), (len)))

#else


#define IMU660RA_SCK(x)				IMU660RA_SPC_PIN  = x
#define IMU660RA_MOSI(x) 			IMU660RA_SDI_PIN = x
#define IMU660RA_CS(x)  			IMU660RA_CS_PIN  = x
#define IMU660RA_MISO    			IMU660RA_SDO_PIN


//-------------------------------------------------------------------------------------------------------------------
//  @brief      通过SPI写一个byte,同时读取一个byte
//-------------------------------------------------------------------------------------------------------------------
static uint8 imu660ra_simspi_wr_byte(uint8 byte)
{
    uint8 xdata i;
    for(i=0; i<8; i++)
    {
        IMU660RA_MOSI(byte&0x80);
        byte <<= 1;
        IMU660RA_SCK (0);
        IMU660RA_SCK (0);
        IMU660RA_SCK (1);
        IMU660RA_SCK (1);
        byte |= IMU660RA_MISO;
    }
    return(byte);
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      通过SPI写命令+数据 (单字节)
//-------------------------------------------------------------------------------------------------------------------
static void imu660ra_simspi_w_reg_byte(uint8 cmd, uint8 val)
{
    cmd &= ~IMU660RA_SPI_R;
    imu660ra_simspi_wr_byte(cmd);
    imu660ra_simspi_wr_byte(val);
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      通过SPI连续写入多字节数据
//-------------------------------------------------------------------------------------------------------------------
static void imu660ra_simspi_w_reg_bytes(uint8 cmd, uint8 *dat_addr, uint32 len)
{
    cmd &= ~IMU660RA_SPI_R;
    imu660ra_simspi_wr_byte(cmd);
    while(len--)
    {
        imu660ra_simspi_wr_byte(*dat_addr++);
    }
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      通过SPI连续读取多字节数据
//  @note       LSM6DSR: 发送读命令后, 第一个返回字节即为寄存器数据(无dummy byte)
//-------------------------------------------------------------------------------------------------------------------
static void imu660ra_simspi_r_reg_bytes(uint8 cmd, uint8 *val, uint32 num)
{
    cmd |= IMU660RA_SPI_R;
    imu660ra_simspi_wr_byte(cmd);
    while(num--)
    {
        *val++ = imu660ra_simspi_wr_byte(0);
    }
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      LSM6DSR 写单个寄存器
//-------------------------------------------------------------------------------------------------------------------
static void imu660ra_write_register(uint8 reg, uint8 dat)
{
    IMU660RA_CS(0);
    imu660ra_simspi_w_reg_byte(reg, dat);
    IMU660RA_CS(1);
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      LSM6DSR 连续写多个寄存器
//-------------------------------------------------------------------------------------------------------------------
static void imu660ra_write_registers(uint8 reg, const uint8 *dat, uint32 len)
{
    IMU660RA_CS(0);
    imu660ra_simspi_w_reg_bytes(reg, dat, len);
    IMU660RA_CS(1);
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      LSM6DSR 读单个寄存器 (无dummy byte)
//-------------------------------------------------------------------------------------------------------------------
static uint8 imu660ra_read_register(uint8 reg)
{
    uint8 xdata dat;
    IMU660RA_CS(0);
    imu660ra_simspi_r_reg_bytes(reg, &dat, 1);
    IMU660RA_CS(1);
    return dat;
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      LSM6DSR 连续读多个寄存器
//-------------------------------------------------------------------------------------------------------------------
static void imu660ra_read_registers(uint8 reg, uint8 *dat, uint32 len)
{
    IMU660RA_CS(0);
    imu660ra_simspi_r_reg_bytes(reg, dat, len);
    IMU660RA_CS(1);
}
#endif


//-------------------------------------------------------------------------------------------------------------------
//  @brief      LSM6DSR 自检 (读取 WHO_AM_I)
//-------------------------------------------------------------------------------------------------------------------
static uint8 imu660ra_self_check (void)
{
    uint8 xdata dat = 0, return_state = 0;
    uint16 xdata timeout_count = 0;
    do
    {
        if(timeout_count ++ > IMU660RA_TIMEOUT_COUNT)
        {
            return_state = 1;
            break;
        }
        dat = imu660ra_read_register(IMU660RA_CHIP_ID);
        delay_ms(1);
    }
    while(0x6B != dat);
    return return_state;
}


//-------------------------------------------------------------------------------------------------------------------
//  @brief      LSM6DSR 量程选择 -> 寄存器 FS 位 映射
//-------------------------------------------------------------------------------------------------------------------
static uint8 imu660ra_map_acc_fs(uint8 sample)
{
    switch(sample)
    {
        case 0x00: return 0x00;    // +-2g   (FS_XL=00)
        case 0x01: return 0x02;    // +-4g   (FS_XL=10)
        case 0x02: return 0x03;    // +-8g   (FS_XL=11)
        case 0x03: return 0x01;    // +-16g  (FS_XL=01)
        default:   return 0x03;
    }
}

static uint8 imu660ra_map_gyr_fs(uint8 sample)
{
    switch(sample)
    {
        case 0x00: return 0x03;    // +-2000dps (FS_G=11)
        case 0x01: return 0x02;    // +-1000dps (FS_G=10)
        case 0x02: return 0x01;    // +-500dps  (FS_G=01)
        case 0x03: return 0x00;    // +-250dps  (FS_G=00)
        case 0x04: return 0x00;    // +-125dps  (fallback)
        default:   return 0x03;
    }
}


//-------------------------------------------------------------------------------------------------------------------
//  @brief      获取 LSM6DSR 加速度计数据 (6字节连续读取)
//-------------------------------------------------------------------------------------------------------------------
void imu660ra_get_acc (void)
{
#if IMU660RA_USE_SOFT_IIC
    uint8 xdata dat[6];
    imu660ra_read_registers(IMU660RA_ACC_ADDRESS, dat, 6);
    imu660ra_acc_x = (int16)(((uint16)dat[1]<<8 | dat[0]));
    imu660ra_acc_y = (int16)(((uint16)dat[3]<<8 | dat[2]));
    imu660ra_acc_z = (int16)(((uint16)dat[5]<<8 | dat[4]));
#else
    uint8 xdata dat[6];
    imu660ra_read_registers(IMU660RA_ACC_ADDRESS, dat, 6);
    imu660ra_acc_x = (int16)(((uint16)dat[1]<<8 | dat[0]));
    imu660ra_acc_y = (int16)(((uint16)dat[3]<<8 | dat[2]));
    imu660ra_acc_z = (int16)(((uint16)dat[5]<<8 | dat[4]));
#endif
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      获取 LSM6DSR 陀螺仪数据 (6字节连续读取)
//-------------------------------------------------------------------------------------------------------------------
void imu660ra_get_gyro (void)
{
#if IMU660RA_USE_SOFT_IIC
    uint8 xdata dat[6];
    imu660ra_read_registers(IMU660RA_GYRO_ADDRESS, dat, 6);
    imu660ra_gyro_x = (int16)(((uint16)dat[1]<<8 | dat[0]));
    imu660ra_gyro_y = (int16)(((uint16)dat[3]<<8 | dat[2]));
    imu660ra_gyro_z = (int16)(((uint16)dat[5]<<8 | dat[4]));
#else
    uint8 xdata dat[6];
    imu660ra_read_registers(IMU660RA_GYRO_ADDRESS, dat, 6);
    imu660ra_gyro_x = (int16)(((uint16)dat[1]<<8 | dat[0]));
    imu660ra_gyro_y = (int16)(((uint16)dat[3]<<8 | dat[2]));
    imu660ra_gyro_z = (int16)(((uint16)dat[5]<<8 | dat[4]));
#endif
}


//-------------------------------------------------------------------------------------------------------------------
//  @brief      将加速度计数值转换为实际物理数据
//-------------------------------------------------------------------------------------------------------------------
float imu660ra_acc_transition (int16 acc_value)
{
    float acc_dat = 0;
    switch((uint8)IMU660RA_ACC_SAMPLE)
    {
        case 0x00: acc_dat = (float)acc_value / 16384; break;  // +-2g
        case 0x01: acc_dat = (float)acc_value / 8192;  break;  // +-4g
        case 0x02: acc_dat = (float)acc_value / 4096;  break;  // +-8g
        case 0x03: acc_dat = (float)acc_value / 2048;  break;  // +-16g
        default:   break;
    }
    return acc_dat;
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      将陀螺仪数据转换为实际物理数据
//-------------------------------------------------------------------------------------------------------------------
float imu660ra_gyro_transition (int16 gyro_value)
{
    float gyro_dat = 0;
    switch(IMU660RA_GYR_SAMPLE)
    {
        case 0x00: gyro_dat = (float)gyro_value / 16.4f;  break;   // +-2000dps
        case 0x01: gyro_dat = (float)gyro_value / 32.8f;  break;   // +-1000dps
        case 0x02: gyro_dat = (float)gyro_value / 65.6f;  break;   // +-500dps
        case 0x03: gyro_dat = (float)gyro_value / 131.2f; break;   // +-250dps
        case 0x04: gyro_dat = (float)gyro_value / 262.4f; break;   // +-125dps
        default:   break;
    }
    return gyro_dat;
}


//-------------------------------------------------------------------------------------------------------------------
//  @brief      初始化 LSM6DSRTR
//  @return     uint8           1-初始化失败  0-初始化成功
//-------------------------------------------------------------------------------------------------------------------
uint8 imu660ra_init (void)
{
    uint8 xdata return_state = 0;
    uint8 xdata test_byte;
    uint8 xdata fs_bits;

    delay_ms(50);

#if (IMU660RA_USE_SOFT_IIC == 0)
    test_byte = imu660ra_read_register(IMU660RA_CHIP_ID);
    printf("LSM6DSR WHO_AM_I=0x%02X\r\n", test_byte);
#endif

    do
    {
        if(imu660ra_self_check())
        {
            printf("LSM6DSR self check error (WHO_AM_I != 0x6B).\r\n");
            return_state = 1;
            break;
        }
        printf("Self-check OK (WHO_AM_I=0x6B).\r\n");

        imu660ra_write_register(IMU660RA_CTRL3_C, 0x44);
        delay_ms(1);

        fs_bits = imu660ra_map_acc_fs(IMU660RA_ACC_SAMPLE);
        test_byte = 0x60 | (fs_bits << 2);
        imu660ra_write_register(IMU660RA_CTRL1_XL, test_byte);
        printf("CTRL1_XL=0x%02X (ODR=104Hz)\r\n", test_byte);

        fs_bits = imu660ra_map_gyr_fs(IMU660RA_GYR_SAMPLE);
        test_byte = 0x70 | (fs_bits << 2);
        imu660ra_write_register(IMU660RA_CTRL2_G, test_byte);
        printf("CTRL2_G=0x%02X (ODR=208Hz)\r\n", test_byte);

        test_byte = imu660ra_read_register(IMU660RA_STATUS_REG);
        printf("STATUS_REG=0x%02X\r\n", test_byte);

        delay_ms(50);
        imu660ra_get_acc();
        imu660ra_get_gyro();
        printf("Initial: ACC=(%d,%d,%d) GYR=(%d,%d,%d)\r\n",
               imu660ra_acc_x, imu660ra_acc_y, imu660ra_acc_z,
               imu660ra_gyro_x, imu660ra_gyro_y, imu660ra_gyro_z);

        printf("LSM6DSR init OK.\r\n");
    }
    while(0);
    return return_state;
}
