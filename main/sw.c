
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

const int lowSwNum = GPIO_NUM_13;
const int highSwNum = GPIO_NUM_12;

const int pupmSwNum = GPIO_NUM_1;
const int inSwNum = GPIO_NUM_2;
const int rinseSwNum = GPIO_NUM_42;
const int drainSwNum = GPIO_NUM_41;
const int storageSwNum = GPIO_NUM_40;

volatile bool pupmSw = false;
volatile bool inSw = false;
volatile bool rinseSw = false;
volatile bool drainSw = false;
volatile bool storageSw = false;

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
