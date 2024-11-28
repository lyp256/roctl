#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/_timeval.h>
#include "freertos/FreeRTOS.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "bat3u.h"
#include "sw.h"
#include "oled.h"
#include "format.h"

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

// 断水时间
volatile time_t noWaterAt = NullTime;

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

void statusStr(char *s, char *msg, int status)
{
    time_t lastAt = MAX(lastMakeWaterEndAt, lastRinseEndAt);
    time_t now = time(NULL);
    time_t makeWaterTime = now - startMakeWaterAt;
    time_t rinseTime = now - startRinseAt;
    time_t noWaterTime = now - noWaterAt;
    time_t standByTime = now - lastAt;

    switch (status)
    {
    case StateStandBy:
        strcpy(s, "待机");
        char wtime[20] = "";
        timeString(wtime, standByTime);
        sprintf(msg, "%lld/%d %s", makeWaterTotalTime, rinseThresholdTime, wtime);
        break;
    case StateNoWater:
        strcpy(s, "缺水");
        sprintf(msg, "%lld/%d Sec", noWaterTime, drainTimeout);
        break;
    case StateMakeWaterDrain:
        strcpy(s, "陈水循环");
        sprintf(msg, "%lld/%d Sec", makeWaterTime, drainTimeout);
        break;
    case StateMakeWater:
        strcpy(s, "制水中");
        sprintf(msg, "%lld/%d Sec", makeWaterTime, maxMakeWaterTime);
        break;
    case StateMakeWaterTimeout:
        strcpy(s, "制水超时");
        sprintf(msg, "%lld/%d Sec", makeWaterTime, maxMakeWaterTime);
        break;
    case StateRinse:
        strcpy(s, "冲洗");
        sprintf(msg, "%lld/%d Sec", rinseTime, rinseTimeout);
        break;
    default:
        break;
    }
}

// 冲洗完成
bool goodRinse(Bat3uResT *tds)
{
    if (tds->Sensor1.TDS == 0 || tds->Sensor2.TDS == 0)
    {
        return false;
    }
    int diff = tds->Sensor1.TDS < 10 - tds->Sensor2.TDS;
    return abs(diff) < 10;
}

// 90% 的脱盐率
short desaltTDS(short in)
{
    return (short)(((int)in) * 100 / desaltRote);
}

// 水质合格
bool goodWater(Bat3uResT *tds)
{
    if (tds->Sensor3.TDS == 0 || tds->Sensor1.TDS == 0)
    {
        return false;
    }
    return tds->Sensor3.TDS < 10 || tds->Sensor3.TDS <= desaltTDS(tds->Sensor1.TDS);
}

int makeWaterState(Bat3uResT *tds)
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
        if (noWaterAt == NullTime)
        {
            noWaterAt = now;
        }

        state = StateNoWater;
        return state;
    }
    else
    {
        noWaterAt = NullTime;
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
        else if (goodWater(tds) || makeWaterTime > drainTimeout)
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
        if (startRinseAt != NullTime && (now - startRinseAt > rinseTimeout || goodRinse(tds)))
        {
            // 冲洗结束
            ESP_LOGI("makeWater", "stop rinse!\n");
            startRinseAt = NullTime;
            makeWaterTotalTime = 0;
            lastRinseEndAt = now;
            state = StateStandBy;
        }
    }
    return state;
}

int lastState = 0;

int makeWater(text_ui_t *ui, Bat3uResT *tds)
{
    switchs_t sw = {false, false, false, false, false};
    int state = makeWaterState(tds);
    switch (state)
    {
    case StateRinse:
        sw.pupmSw = true;
        sw.inSw = true;
        sw.rinseSw = true;
        sw.drainSw = false;
        sw.storageSw = false;
        break;
    case StateMakeWater:

        sw.pupmSw = true;
        sw.inSw = true;
        sw.rinseSw = false;
        sw.drainSw = false;
        sw.storageSw = true;
        break;
    case StateMakeWaterDrain:
        sw.pupmSw = true;
        sw.inSw = true;
        sw.rinseSw = false;
        sw.drainSw = true;
        sw.storageSw = false;
        break;
    case StateMakeWaterTimeout:
        sw.pupmSw = false;
        sw.inSw = false;
        sw.rinseSw = false;
        sw.drainSw = false;
        sw.storageSw = false;
        break;
    case StateStandBy:
        sw.pupmSw = false;
        sw.inSw = false;
        sw.rinseSw = false;
        sw.drainSw = false;
        sw.storageSw = false;
        break;
    case StateNoWater:
        sw.pupmSw = false;
        sw.inSw = false;
        sw.rinseSw = false;
        sw.drainSw = false;
        sw.storageSw = false;
        break;
    default:
        sw.pupmSw = false;
        sw.inSw = false;
        sw.rinseSw = false;
        sw.drainSw = false;
        sw.storageSw = false;
        ESP_LOGE("makeWater", "unknown state");
    }

    lastState = state;
    syncSwGPIOLevel(sw);

    char s[32] = "";
    char msg[32] = "";
    statusStr(s, msg, state);
    set_status_ui(ui, s, msg);

    return state;
}
