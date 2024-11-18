#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "bat3u.h"

const int BatErrSuccess = 0;           // 0 成功
const int BatErrTimeOut = -1;          // -1 读取结果超时
const int BatErrInvalidCMD = 1;        // 1 命令帧异常
const int BatErrDeviceBusy = 2;        // 2 忙碌中 03：
const int BatErrCalibrationFailed = 3; // 3 校正失败
const int BatErrTempOverflow = 4;      // 4 检测温度超出范围

static esp_err_t UartCall(Uart *pin, void *send, int sendSize, void *receive, int receiveSize, int *receiveN);

short parseShort(char bytes[2])
{
    return (short)(bytes[0]) << 8 | (short)(bytes[1]);
}

void ZeroData(Bat3uData *res)
{
    res->Sensor1.TDS = 0;
    res->Sensor1.TEMP = 0;
    res->Sensor2.TDS = 0;
    res->Sensor2.TEMP = 0;
    res->Sensor3.TDS = 0;
    res->Sensor3.TEMP = 0;
}

void parseSensorsResult(void *data, Bat3uData *res)
{
    ZeroData(res);
    res->Sensor1.TDS = parseShort(data);
    res->Sensor1.TEMP = parseShort(data + 2);
    res->Sensor2.TDS = parseShort(data + 4);
    res->Sensor2.TEMP = parseShort(data + 6);
    res->Sensor3.TDS = parseShort(data + 8);
    res->Sensor3.TEMP = parseShort(data + 10);
}

int GetBat3uData(Uart *uart, Bat3uData *res)
{
    const char directive[6] = "\xA0\x00\x00\x00\x00\xA0";

    // data simple:AA 11 12 13 14 21 22 23 24 31 32 33 34 76
    const int dataSize = 14;
    char data[14];
    int length;
    esp_err_t resCode;
    resCode = UartCall(uart, directive, 6, data, dataSize, &length);
    if (resCode != ESP_OK)
    {
        return resCode;
    }
    parseSensorsResult(&data[1], res);

    return BatErrSuccess;
}

esp_err_t InitUart(Uart *pin)
{
    uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };

    // Configure UART parameters
    esp_err_t resCode = uart_param_config(pin->uartNum, &uart_config);
    if (resCode != ESP_OK)
    {
        return resCode;
    }

    resCode = uart_set_pin(pin->uartNum, pin->txNum, pin->rxNum, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    if (resCode != ESP_OK)
    {
        return resCode;
    }

    // Setup UART buffered IO with event queue
    const int uart_buffer_size = 1 << 10;
    QueueHandle_t uart_queue;

    // Install UART driver using an event queue here
    resCode = uart_driver_install(pin->uartNum, uart_buffer_size, uart_buffer_size, 10, &uart_queue, 0);
    if (resCode != ESP_OK)
    {
        return resCode;
    }
    return ESP_OK;
}

static esp_err_t UartCall(Uart *pin, void *send, int sendSize, void *receive, int receiveSize, int *receiveN)
{
    int length = length = uart_write_bytes(pin->uartNum, (const char *)send, sendSize);
    if (length != sendSize)
    {
        // todo
    }

    // Read data from UART.
    length = 0;
    for (long i = 0; i < 10; i++)
    {

        esp_err_t resCode = uart_get_buffered_data_len(pin->uartNum, (size_t *)&length);
        if (resCode != ESP_OK)
        {
            return resCode;
        }
        if (length > 0)
        {
            break;
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    if (length == 0)
    {
        return ESP_ERR_TIMEOUT;
    }

    if (receiveSize < length)
    {
        length = receiveSize;
    }

    length = uart_read_bytes(pin->uartNum, receive, length, 100);
    *receiveN = length;
    return 0;
}

void PrintBat3uData(Bat3uData *data)
{
    printf("Sensor1.TDS:%d\n", data->Sensor1.TDS);
    printf("Sensor1.TEMP:%d\n", data->Sensor1.TEMP);
    printf("Sensor2.TDS:%d\n", data->Sensor2.TDS);
    printf("Sensor2.TEMP:%d\n", data->Sensor2.TEMP);
    printf("Sensor3.TDS:%d\n", data->Sensor3.TDS);
    printf("Sensor3.TEMP:%d\n", data->Sensor3.TEMP);
}