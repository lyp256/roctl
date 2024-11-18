#ifndef _SW_H_
#define _SW_H_
#include "driver/gpio.h"

extern volatile bool pupmSw;
extern volatile bool inSw;
extern volatile bool rinseSw;
extern volatile bool drainSw;
extern volatile bool storageSw;

void initSwGPIO();
void syncSwGPIOLevel();
int highSw();
int lowSw();
#endif