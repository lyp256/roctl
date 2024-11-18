#ifndef _LED_H__
#define _LED_H__

#include "freertos/FreeRTOS.h"

typedef struct
{
    uint8_t r;
    uint8_t g;
    uint8_t b;

} color_t;

typedef struct
{
    color_t color;   // 颜色
    uint16_t frames; // 持续的帧数，一帧代表 1/25 秒
} ledFrame;

typedef struct
{
    ledFrame *data; // 数据
    size_t len;
} ledPlays;

extern const color_t redColor;
extern const color_t greenColor;
extern const color_t blueColor;
extern const color_t yellowColor;
extern const color_t cyanColor;
extern const color_t purpleColor;
extern const color_t orangeColor;
extern const color_t whiteColor;
extern const color_t noColor;

void rgbLED(color_t rgb);
void configureLED(void);
void blinkPlayBook(ledPlays *play, color_t rgb, uint16_t d);
void alwaysOnPlayBook(ledPlays *play, color_t rgb);
ledFrame *makeFrames(size_t n);
int setPlayBook(ledPlays *play, ledFrame *frames, size_t n);
void ledLoopTask(ledPlays *playBook);
#endif