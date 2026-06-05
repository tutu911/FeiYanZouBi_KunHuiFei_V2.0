#ifndef __MENU_H
#define __MENU_H

#include "headfile.h"

// 按键编码
typedef enum {
    KEY_NONE = 0,
    KEY_ADD  = 1,    // 增加
    KEY_DOWN = 2,    // 下移
    KEY_REDUCE = 3,  // 减少
    KEY_BACK   = 4,  // 退出(二级菜单)
    KEY_ENTER  = 5   // 进入(一级菜单确认)
} KeyCode;

typedef enum {
    KEY_STATE_RELEASED = 0,
    KEY_STATE_PRESSED,
} KeyState;

extern uint8 menu_flag;
extern uint8 KeyNum;
extern int16 ADC_BAT;
extern uint16 Key_Adc;


uint8 Key_Scan(void);
void ADC_Show(void);
void MODE_Show(void);
void FirstMenu_ips114(void);
void SecondMenu_ips114(void);
void Menu_Init(void);


#endif