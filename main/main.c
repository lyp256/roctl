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
#include "led.h"
#include "makewater.h"
#include "oled.h"
#include "pin.h"

void app_main(void)
{
    ESP_LOGI("TDS", "start sync tds!\n");
    UartT pin = {
        .uartNum = UART_NUM_1,
        .txNum = TDSTX,
        .rxNum = TDSRX,
    };
    InitUart(&pin);
    ledPlays_t ledPlayBook = {NULL, 0};
    xTaskCreate(ledLoopTask, "ledLoopTask", 10240, &ledPlayBook, 1, NULL);
    xTaskCreate(oled, "oled", 10240, NULL, 1, NULL);
    initSwGPIO();
    for (uint32_t i = 0; true; i++)
    {
        syncTDS(&pin);
        makeWater(&ledPlayBook);
        vTaskDelay(pdMS_TO_TICKS(300));
    }
}
