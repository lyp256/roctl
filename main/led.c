
#include "freertos/FreeRTOS.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "led_strip.h"
#include "led.h"

#define BLINK_GPIO GPIO_NUM_48
#define FRAMETIME 40 // 帧持续时间毫秒

static led_strip_handle_t led_strip;

const color_t redColor = {255, 0, 0};
const color_t greenColor = {0, 255, 0};
const color_t blueColor = {0, 0, 255};
const color_t yellowColor = {255, 255, 0};
const color_t cyanColor = {0, 255, 255};
const color_t purpleColor = {255, 0, 255};
const color_t orangeColor = {255, 165, 0};
const color_t whiteColor = {255, 255, 255};
const color_t noColor = {0, 0, 0};

void rgbString(char *s, color_t rgb)
{
    sprintf(s, "%02X%02X%02X", rgb.r, rgb.b, rgb.b);
}

void rgbLED(color_t rgb)
{
    char cs[7];
    rgbString(cs, rgb);
    ESP_LOGV("led", "color:%s\n", cs);
    if (rgb.r == 0 && rgb.g == 0 && rgb.b == 0)
    {
        led_strip_clear(led_strip);
        return;
    }

    led_strip_set_pixel(led_strip, 0, rgb.r, rgb.g, rgb.b);
    led_strip_refresh(led_strip);
}

void configureLED(void)
{
    led_strip_config_t strip_config = {
        .strip_gpio_num = BLINK_GPIO,
        .max_leds = 1,
    };
    led_strip_rmt_config_t rmt_config = {
        .resolution_hz = 10 * 1000 * 1000, // 10MHz
        .flags.with_dma = false,
    };
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));

    /* Set all LED off to clear all pixels */
    led_strip_clear(led_strip);
}

void resetLEDPlays(ledPlays *play)
{
    if (play->data != NULL)
    {
        free(play->data);
        play->data = NULL;
    }
    play->len = 0;
}

void blinkPlayBook(ledPlays *play, color_t rgb, uint16_t d)
{
    if (play->data != NULL)
    {
        resetLEDPlays(play);
    }
    play->data = malloc(sizeof(ledFrame) * 2);
    ledFrame *p = play->data;
    p[0]=(ledFrame){rgb,d};
    p[1]=(ledFrame){noColor,d};
    play->len = 2;
}

void alwaysOnPlayBook(ledPlays *play, color_t rgb)
{
    if (play->data != NULL)
    {
        resetLEDPlays(play);
    }
    play->data = makeFrames(1);
    ledFrame *p = play->data;
    p[0]=(ledFrame){rgb,25};
    play->len = 1;
}

ledFrame *makeFrames(size_t n)
{
    return malloc(sizeof(ledFrame) * n);
};

int setPlayBook(ledPlays *play, ledFrame *frames, size_t n)
{
    size_t total= sizeof(frames);
    if (total / sizeof(ledFrame) < n)
    {
        return 1;
    }
    if (play->data != NULL)
    {
        resetLEDPlays(play);
    }
    play->data = frames;
    play->len = n;
    return 0;
}

void ledLoopTask(ledPlays *playBook)
{
    configureLED();
    while (1)
    {
    playStart:
        if (playBook == NULL)
        {
            rgbLED(noColor);
            vTaskDelay(pdMS_TO_TICKS(FRAMETIME));
            continue;
        }
        ledFrame *data = playBook->data;

        for (size_t i = 0; i < playBook->len; i++)
        {
            ledFrame *curr = data + i;
            color_t rgb = curr->color;
            char cs[7];
            rgbString(cs, rgb);

            rgbLED(rgb);
            if (curr->frames < 1)
            {
                curr->frames = 1;
            }
            ESP_LOGI("led", "color:%s\n,frame:%d,duration:%d ms\n", cs, curr->frames, curr->frames * FRAMETIME);
            for (size_t i = 0; i < curr->frames; i++)
            {
                vTaskDelay(pdMS_TO_TICKS(FRAMETIME));
                if (data != playBook->data)
                {
                    ESP_LOGI("led", "change play book");
                    goto playStart;
                }
            }
        }
        if (data == NULL)
        {
            rgbLED(noColor);
            vTaskDelay(pdMS_TO_TICKS(FRAMETIME));
        }
    }
}