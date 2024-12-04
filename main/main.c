#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/_timeval.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "bat3u.h"
#include "sw.h"
#include "makewater.h"
#include "oled.h"
#include "pin.h"

void getTDSData(uart_t *pin, Bat3uResT *tds)
{
    Bat3uResT res;
    uint32_t now = esp_log_timestamp();
    int resCode = GetBat3uData(pin, &res);
    ESP_LOGV("TDS", "GetBat3uData cost time: %ld", esp_log_timestamp() - now);
    if (resCode != 0)
    {
        ESP_LOGE("TDS", "GetBat3uData:%d\n", resCode);
        return;
    }
    tds->Sensor1 = res.Sensor1;
    tds->Sensor2 = res.Sensor2;
    tds->Sensor3 = res.Sensor3;
}

void app_main(void)
{
    // switch init
    initSwGPIO();

    // tds uart
    uart_t pin = {
        .uartNum = UART_NUM_1,
        .txNum = TDSTX,
        .rxNum = TDSRX,
    };
    Bat3uResT tds;
    ZeroData(&tds);
    InitUart(&pin);
    text_ui_t ui;
    init_oled(&ui);
    set_tds_ui(&ui, &tds);
    for (uint32_t i = 0; true; i++)
    {
        getTDSData(&pin, &tds);
        set_tds_ui(&ui, &tds);
        makeWater(&ui,&tds);
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}
