#ifndef _SW_H_
#define _SW_H_
#include "driver/gpio.h"

typedef struct
{
    bool pupmSw;    // 压力泵
    bool inSw;      // 进水开关
    bool rinseSw;   // 冲洗开关
    bool drainSw;   // 陈水回流开关
    bool storageSw; // 储水桶开关
} switchs_t;

void initSwGPIO();
void syncSwGPIOLevel(switchs_t s);
int highSw();
int lowSw();
#endif