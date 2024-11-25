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
#include "oled.h"

#define StateStandBy 1
#define StateNoWater 2
#define StateMakeWaterDrain 3
#define StateMakeWater 4
#define StateMakeWaterTimeout 5
#define StateRinse 6

#define drainTimeout 15       // 最大陈水排放时间，秒
#define rinseTimeout 15       // 最大冲洗时间，秒
#define maxMakeWaterTime 1800 // 最大制水时间，防止水路漏水导致不停制水
#define rinseThresholdTime 60 // 运行多久后开始冲洗，秒
#define desaltRote 95         // 脱盐率，百分比

#define NullTime -1

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
    if (tdsData.Sensor3.TDS == 0||tdsData.Sensor1.TDS == 0)
    {
        return false;
    }
    return tdsData.Sensor3.TDS < 10 || tdsData.Sensor3.TDS <= desaltTDS(tdsData.Sensor1.TDS);
}

int makeWaterState()
{
    int state = StateStandBy;
    time_t now = time(NULL);
    bool low = lowSw();
    bool high = highSw();
    ESP_LOGV("water", "low:%d,high:%d\n", low, high);

    // 无水停止所有动作
    if (low)
    {
        if (startRinseAt != NullTime)
        {
            ESP_LOGI("makeWater", "stop rinse: without input water \n");
            startRinseAt = NullTime;
        }
        if (startMakeWaterAt != NullTime)
        {
            ESP_LOGI("makeWater", "stop make water:  without input water \n");
            lastMakeWaterEndAt = now;
            makeWaterTotalTime += lastMakeWaterEndAt - startMakeWaterAt;
            startMakeWaterAt = NullTime;
        }
        state = StateNoWater;
        return state;
    }

    // 制水判断
    if (high)
    {
        if (startRinseAt != NullTime)
        {
            ESP_LOGI("makeWater", "stop rinse");
        }

        // 制水逻辑
        startRinseAt = NullTime;
        if (startMakeWaterAt == NullTime)
        {
            ESP_LOGI("makeWater", "start make water\n");
            startMakeWaterAt = now;
        }
        time_t makeWaterTime = now - startMakeWaterAt;

        if (makeWaterTime > maxMakeWaterTime)
        {
            state = StateMakeWaterTimeout;
        }
        else if (goodWater() || makeWaterTime > drainTimeout)
        {
            state = StateMakeWater;
        }
        else
        {
            state = StateMakeWaterDrain;
        }
    }
    else
    {
        // 制水停止
        if (startMakeWaterAt != NullTime)
        {
            ESP_LOGI("makeWater", "stop make water\n");
            lastMakeWaterEndAt = now;
            makeWaterTotalTime += lastMakeWaterEndAt - startMakeWaterAt;
            startMakeWaterAt = NullTime;
        }

        if (makeWaterTotalTime > rinseThresholdTime)
        {
            // 需要冲洗
            if (startRinseAt == NullTime)
            {
                ESP_LOGI("makeWater", "start rinse\n");
                startRinseAt = now;
            }
            state = StateRinse;
        }
        else
        {
            // 不需要冲洗
            state = StateStandBy;
        }
        if (startRinseAt != NullTime && (now - startRinseAt > rinseTimeout || goodRinse()))
        {
            // 冲洗结束
            ESP_LOGI("makeWater", "stop rinse!\n");
            startRinseAt = NullTime;
            makeWaterTotalTime = 0;
            lastRinseEndAt = now;
            state = StateRinse;
        }
    }
    return state;
}

int lastState = 0;

void makeWater(ledPlays_t *ledPlayBook)
{
    ledFrame_t *frames = makeFrames(2);
    int framesLen = 2;

    int state = makeWaterState();
    switch (state)
    {
    case StateRinse:
        sprintf(uiStatusText, "Rinse");
        setFrame(&frames[0], yellowColor, 10);
        setFrame(&frames[1], noColor, 10);
        pupmSw = true;
        inSw = true;
        rinseSw = true;
        drainSw = false;
        storageSw = false;
        break;
    case StateMakeWater:
        sprintf(uiStatusText, "MakeWater");
        setFrame(&frames[0], blueColor, 10);
        setFrame(&frames[1], blueColor, 10);
        pupmSw = true;
        inSw = true;
        rinseSw = false;
        drainSw = false;
        storageSw = true;
        break;
    case StateMakeWaterDrain:
        sprintf(uiStatusText, "MakeWaterDrain");
        setFrame(&frames[0], blueColor, 10);
        setFrame(&frames[1], yellowColor, 10);
        pupmSw = true;
        inSw = true;
        rinseSw = false;
        drainSw = true;
        storageSw = false;
        break;
    case StateMakeWaterTimeout:
        sprintf(uiStatusText, "MakeTimeout");
        setFrame(&frames[0], blueColor, 10);
        setFrame(&frames[1], redColor, 10);
        pupmSw = false;
        inSw = false;
        rinseSw = false;
        drainSw = false;
        storageSw = false;
        break;
    case StateStandBy:
        sprintf(uiStatusText, "StandBy");
        setFrame(&frames[0], greenColor, 10);
        setFrame(&frames[1], greenColor, 10);
        pupmSw = false;
        inSw = false;
        rinseSw = false;
        drainSw = false;
        storageSw = false;
        break;
    case StateNoWater:
        uiStatusText = "NoInWater";
        setFrame(&frames[0], redColor, 10);
        setFrame(&frames[1], noColor, 10);
        pupmSw = false;
        inSw = false;
        rinseSw = false;
        drainSw = false;
        storageSw = false;
        break;
    default:
        pupmSw = false;
        inSw = false;
        rinseSw = false;
        drainSw = false;
        storageSw = false;
        ESP_LOGE("makeWater", "unknown state");
    }
    if (lastState != state)
    {
        setPlayBook(ledPlayBook, frames, framesLen);
        ESP_LOGI("makeWatrt","%d to %d",lastState,state);
    }
    else
    {
        free(frames);
    }
    lastState = state;
    syncSwGPIOLevel();
}
