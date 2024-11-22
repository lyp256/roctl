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
} ledFrame_t;

typedef struct
{
    ledFrame_t *data; // 数据
    size_t len;
} ledPlays_t;

extern const color_t redColor;
extern const color_t greenColor;
extern const color_t blueColor;
extern const color_t yellowColor;
extern const color_t cyanColor;
extern const color_t purpleColor;
extern const color_t orangeColor;
extern const color_t whiteColor;
extern const color_t noColor;

void configureLED(void);
void blinkPlayBook(ledPlays_t *play, color_t rgb, uint16_t d);
void alwaysOnPlayBook(ledPlays_t *play, color_t rgb);
ledFrame_t *makeFrames(size_t n);
void setPlayBook(ledPlays_t *play, ledFrame_t *frames, size_t n);
void setFrame(ledFrame_t *frame, color_t color, uint16_t fc);
void ledLoopTask(ledPlays_t *playBook);
#endif