
#include <stdio.h>
#include <sys/time.h>
#include <sys/_timeval.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "sw.h"
#include "pin.h"

volatile bool pupmSw = false;    // 压力泵
volatile bool inSw = false;      // 进水开关
volatile bool rinseSw = false;   // 冲洗开关
volatile bool drainSw = false;   // 陈水回流开关
volatile bool storageSw = false; // 储水桶开关

int lowSw()
{
    return gpio_get_level(lowSwNum);
}

int highSw()
{
    return gpio_get_level(highSwNum);
}

void initSwGPIO()
{
    int outs[] = {pupmSwNum, inSwNum, rinseSwNum, drainSwNum, storageSwNum};
    int ins[] = {lowSwNum, highSwNum};
    int outsLen = sizeof(outs) / sizeof(outs[0]);
    int insLen = sizeof(ins) / sizeof(ins[0]);
    // init output
    for (int i = 0; i < outsLen; i++)
    {
        int num = outs[i];
        gpio_reset_pin(num);
        gpio_set_direction(num, GPIO_MODE_OUTPUT);
        gpio_set_level(num, 0);
    }
    // init input
    for (int i = 0; i < insLen; i++)
    {
        int num = ins[i];
        gpio_config_t cfg = {
            .pin_bit_mask = BIT64(num),
            .mode = GPIO_MODE_INPUT,
            .pull_up_en = false,
            .pull_down_en = true,
            .intr_type = GPIO_INTR_DISABLE,
        };
        gpio_config(&cfg);
    }
}

void syncSwGPIOLevel()
{
    gpio_set_level(inSwNum, inSw);
    gpio_set_level(pupmSwNum, pupmSw);
    gpio_set_level(rinseSwNum, rinseSw);
    gpio_set_level(drainSwNum, drainSw);
    gpio_set_level(storageSwNum, storageSw);
}
