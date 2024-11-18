#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/_timeval.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "bat3u.h"
#include "sw.h"
#include "led.h"

#define NullTime -1
#define drainTimeout 15       // 最大陈水排放时间，秒
#define rinseTimeout 15       // 最大冲洗时间，秒
#define rinseThresholdTime 60 // 运行多久后开始冲洗，秒
#define desaltRote 95         // 脱盐率，百分比

// TDS 数据
// Sensor1 in tds
// Sensor2 dirain tds
// sensor3 out tds
volatile Bat3uData tdsData;

// 开始制水时间
volatile time_t startMakeWaterAt = NullTime;

// 最近制水结束时间
volatile time_t lastMakeWaterEndAt = NullTime;

// 开始冲洗时间
volatile time_t startRinseAt = NullTime;

// 最后冲洗时间
volatile time_t lastRinseEndAt = NullTime;

// 距离上一次冲洗后总计制水时间
volatile time_t makeWaterTotalTime = 0;

// 冲洗完成
bool goodRinse()
{
    if (tdsData.Sensor1.TDS == 0 || tdsData.Sensor2.TDS == 0)
    {
        return false;
    }
    int diff = tdsData.Sensor1.TDS < 10 - tdsData.Sensor2.TDS;
    return abs(diff) < 10;
}

// 90% 的脱盐率
short desaltTDS(short in)
{
    return (short)(((int)in) * 100 / desaltRote);
}

// 水质合格
bool goodWater()
{
    if (tdsData.Sensor1.TDS == 0)
    {
        return false;
    }
    return tdsData.Sensor3.TDS < 10 || tdsData.Sensor3.TDS <= desaltTDS(tdsData.Sensor1.TDS);
}

void makeWater()
{
    time_t now = time(NULL);
    bool low = lowSw();
    bool high = highSw();
    ESP_LOGI("water", "low:%d,high:%d\n", low, high);

    // 无水停止所有动作
    if (low)
    {
        if (startRinseAt != NullTime)
        {
            esp_rom_printf("stop rinse: without input water \n");
            startRinseAt = NullTime;
        }
        if (startMakeWaterAt != NullTime)
        {
            esp_rom_printf("stop make water:  without input water \n");
            lastMakeWaterEndAt = now;
            makeWaterTotalTime += lastMakeWaterEndAt - startMakeWaterAt;
            startMakeWaterAt = NullTime;
        }
        pupmSw = false;
        inSw = false;
        rinseSw = false;
        drainSw = false;
        storageSw = false;
        syncSwGPIOLevel();
        return;
    }

    // 制水判断
    if (high)
    {
        if (startRinseAt != NullTime)
        {
            esp_rom_printf("stop rinse\n");
            startRinseAt = NullTime;
        }

        // 制水逻辑
        startRinseAt = NullTime;
        if (startMakeWaterAt == NullTime)
        {
            esp_rom_printf("start make water\n");
            startMakeWaterAt = now;
        }
        time_t makeWaterTime = now - startMakeWaterAt;
        inSw = true;
        pupmSw = true;
        rinseSw = false;
        if (goodWater() || makeWaterTime > drainTimeout)
        {
            storageSw = true;
            drainSw = false;
        }
        else
        {
            storageSw = false;
            drainSw = true;
        }
    }
    else
    {
        // 制水停止
        if (startMakeWaterAt != NullTime)
        {
            esp_rom_printf("stop make water\n");
            lastMakeWaterEndAt = now;
            makeWaterTotalTime += lastMakeWaterEndAt - startMakeWaterAt;
            startMakeWaterAt = NullTime;
        }
        storageSw = false;
        drainSw = false;

        if (makeWaterTotalTime > rinseThresholdTime)
        {
            // 需要冲洗
            if (startRinseAt != NullTime)
            {
                esp_rom_printf("start rinse\n");
                startRinseAt = now;
            }
            rinseSw = true;
            inSw = true;
            pupmSw = true;
        }
        else
        {
            // 不需要冲洗
            rinseSw = false;
            inSw = false;
            pupmSw = false;
        }
        if (startRinseAt != NullTime && (now - startRinseAt > rinseTimeout || goodRinse()))
        {
            // 冲洗结束
            esp_rom_printf("stop rinse!\n");
            startRinseAt = NullTime;
            makeWaterTotalTime = 0;
            lastRinseEndAt = now;
            rinseSw = false;
            inSw = false;
            pupmSw = false;
        }
    }

    syncSwGPIOLevel();
}

void syncTDS(void)
{
    printf("start sync tds!\n");

    Uart pin = {
        .uartNum = UART_NUM_2,
        .txNum = 17,
        .rxNum = 16,
    };
    InitUart(&pin);
    for (int i = 0; true; i++)
    {
        Bat3uData res;
        ZeroData(&res);
        printf("befor\n");
        PrintBat3uData(&res);
        int resCode = GetBat3uData(&pin, &res);
        if (resCode != 0)
        {
            printf("GetBat3uData:%d\n", resCode);
        }
        else
        {
            printf("after\n");
            PrintBat3uData(&res);
        }
        printf("\n");
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

ledPlays ledPlayBook = {NULL, 0};

void app_main(void)
{

    ledFrame *frames = (ledFrame *)makeFrames(9);
    frames[0] = (ledFrame){.color = redColor, .frames = 2};
    frames[1] = (ledFrame){.color = greenColor, .frames = 2};
    frames[2] = (ledFrame){.color = blueColor, .frames = 2};
    frames[3] = (ledFrame){.color = yellowColor, .frames = 2};
    frames[4] = (ledFrame){.color = cyanColor, .frames = 2};
    frames[5] = (ledFrame){.color = purpleColor, .frames = 2};
    frames[6] = (ledFrame){.color = orangeColor, .frames = 2};
    frames[7] = (ledFrame){.color = whiteColor, .frames = 2};
    frames[8] = (ledFrame){.color = noColor, .frames = 2};
    setPlayBook(&ledPlayBook, frames, 9);
    xTaskCreate(ledLoopTask, "ledLoopTask", 10240, &ledPlayBook, 1, NULL);
    initSwGPIO();
    for (uint32_t i = 0; true; i++)
    {
        makeWater();

        vTaskDelay(pdMS_TO_TICKS(1500));
    }
}
