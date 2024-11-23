#include "driver/gpio.h"

#define LED_GPIO GPIO_NUM_5 // rgb led gpio

#define lowSwNum GPIO_NUM_6  // 低压开关
#define highSwNum GPIO_NUM_7 // 高压开关

#define pupmSwNum GPIO_NUM_0    // 压力泵
#define inSwNum GPIO_NUM_1      // 进水
#define rinseSwNum GPIO_NUM_2   // 冲洗
#define drainSwNum GPIO_NUM_3   // 沉水回流
#define storageSwNum GPIO_NUM_4 // 压力开关泵

#define TDSRX GPIO_NUM_21  // tds UartT rx
#define TDSTX GPIO_NUM_20  // tds UartT tx
#define TDSUART UART_NUM_1 // tds uart chan

#define OLEDSDA GPIO_NUM_8 // oled iic sda
#define OLEDSCL GPIO_NUM_9 // oled iic scl