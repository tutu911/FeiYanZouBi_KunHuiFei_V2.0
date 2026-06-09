/*********************************************************************************************************************
 * @file       		Protocol.c
 * @description 		上位机(VOFA+) PID调参协议模块
 *                  通过无线串口接收上位机发送的PID参数指令，
 *                  解析并更新MCU端对应变量，替代低效的按键调参。
 * @protocol   		文本指令，换行分隔: KEY=VALUE\n
 *                  支持 '=', ':', ' ' 作为分隔符
 ********************************************************************************************************************/

#include "Protocol.h"

//------ 外部PID参数变量声明 ------
extern float P_dir;          // 方向PD参数 (Menu菜单2)
extern float D_dir;
extern float P_Sp;           // 速度环PD参数 (Menu菜单3)
extern float D_Sp;
extern float P_Gy;           // 角速度环PI参数 (Menu菜单3)
extern float I_Gy;
extern int Target_Speed;     // 目标速度
extern int16 Distance_all;   // 固定运行距离
extern int Cirque_Speed;     // 圆环速度
extern int Cirque_GYRO;      // 圆环角速度阈值
extern int Cirque_ROAD;      // 圆环路径
extern uint16 Target_Pwmin;  // 目标占空比

//------ 外部函数声明 ------
extern void Write_EEPROM(void);       // pid.c: EEPROM写入
extern float Str2num(char *str_ptr);  // flash.c: 字符串→float
extern void Num2str(float NUM);       // flash.c: float→字符串(存入Arry)
extern char Arry[10];                 // flash.c: Num2str输出缓冲区

//------ 接收缓冲区 ------
#define RX_BUF_SIZE 64
static char rx_buf[RX_BUF_SIZE];
static uint8 rx_idx = 0;


/**
 * @description: 发送确认回执到上位机
 * @param key   参数名
 * @param val   更新后的值
 */
static void Protocol_Echo(const char *key, float val)
{
    uint8 i;
    // 发送 "OK:" + key + "="
    uart_putstr(UART_3, (uint8 *)"OK:");
    uart_putstr(UART_3, (uint8 *)key);
    uart_putchar(UART_3, '=');

    // 使用Num2str将float转为字符串(结果在Arry[]中)，然后发送
    Num2str(val);
    for (i = 0; i < 10; i++)
    {
        if (Arry[i] == '\0') break;
        uart_putchar(UART_3, Arry[i]);
    }
    uart_putstr(UART_3, (uint8 *)"\r\n");
}


/**
 * @description: 解析单行指令，更新对应PID参数
 * @param line  以'\0'结尾的指令行
 */
static void Protocol_ParseLine(char *line)
{
    char *p;
    char *key;
    char *val;
    char *sep = NULL;
    uint8 matched = 0;
    float fval;

    // 去除前导空白
    while (*line == ' ' || *line == '\t') line++;
    if (*line == '\0') return;

    // 查找分隔符 '=' 或 ':' 或 ' '
    for (p = line; *p != '\0'; p++)
    {
        if (*p == '=' || *p == ':' || *p == ' ')
        {
            sep = p;
            break;
        }
    }

    if (sep != NULL)
    {
        // 有分隔符 → KEY=VALUE 格式
        *sep = '\0';
        key = line;
        val = sep + 1;
        while (*val == ' ') val++;  // 跳过值前的空格

        fval = Str2num(val);
        matched = 1;

        // 匹配参数名并更新对应变量
        if (strcmp(key, "P_dir") == 0)
            P_dir = fval;
        else if (strcmp(key, "D_dir") == 0)
            D_dir = fval;
        else if (strcmp(key, "P_Sp") == 0)
            P_Sp = fval;
        else if (strcmp(key, "D_Sp") == 0)
            D_Sp = fval;
        else if (strcmp(key, "P_Gy") == 0)
            P_Gy = fval;
        else if (strcmp(key, "I_Gy") == 0)
            I_Gy = fval;
        else if (strcmp(key, "Target_Speed") == 0 || strcmp(key, "TS") == 0)
            Target_Speed = (int)fval;
        else if (strcmp(key, "Distance") == 0 || strcmp(key, "Dist") == 0)
            Distance_all = (int16)fval;
        else if (strcmp(key, "Cirque_Speed") == 0 || strcmp(key, "CS") == 0)
            Cirque_Speed = (int)fval;
        else if (strcmp(key, "Cirque_GYRO") == 0 || strcmp(key, "CG") == 0)
            Cirque_GYRO = (int)fval;
        else if (strcmp(key, "Cirque_ROAD") == 0 || strcmp(key, "CR") == 0)
            Cirque_ROAD = (int)fval;
        else if (strcmp(key, "Target_Pwmin") == 0 || strcmp(key, "PW") == 0)
            Target_Pwmin = (uint16)fval;
        else
            matched = 0;

        if (matched)
        {
            Protocol_Echo(key, fval);
        }
    }
    else
    {
        // 无分隔符 → 检查特殊命令
        if (strcmp(line, "save") == 0 || strcmp(line, "SAVE") == 0)
        {
            Write_EEPROM();
            uart_putstr(UART_3, (uint8 *)"SAVED\r\n");
        }
    }
}


/**
 * @description: 协议主处理函数，在主循环中调用
 *               从无线串口FIFO读取数据，按行解析指令
 */
void Protocol_Process(void)
{
    uint8 ch;

    // 读取FIFO中所有可用字节
    while (wireless_uart_read_buff(&ch, 1) == 1)
    {
        if (ch == '\n')
        {
            // 换行符 → 处理已缓冲的行
            if (rx_idx > 0)
            {
                // 去除末尾的'\r'（处理\r\n）
                if (rx_idx > 0 && rx_buf[rx_idx - 1] == '\r')
                    rx_buf[rx_idx - 1] = '\0';
                else
                    rx_buf[rx_idx] = '\0';

                Protocol_ParseLine(rx_buf);
                rx_idx = 0;
            }
        }
        else if (ch == '\r')
        {
            // 回车符 → 也可能作为行结束符
            if (rx_idx > 0)
            {
                rx_buf[rx_idx] = '\0';
                Protocol_ParseLine(rx_buf);
                rx_idx = 0;
            }
        }
        else if (rx_idx < RX_BUF_SIZE - 1)
        {
            // 普通字符 → 加入缓冲区
            rx_buf[rx_idx++] = ch;
        }
        else
        {
            // 缓冲区溢出保护 → 丢弃当前行
            rx_idx = 0;
        }
    }
}
