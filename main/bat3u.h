#ifndef _BAT3U_H_
#define _BAT3U_H_
#include "driver/uart.h"
typedef struct
{
    short TDS;  // 单位 ppm
    short TEMP; // 单位 0.01 摄氏度
} SensorDataT;

typedef struct
{
    SensorDataT Sensor1;
    SensorDataT Sensor2;
    SensorDataT Sensor3;
} Bat3uResT;

typedef struct
{
    uart_port_t uartNum;
    int txNum, rxNum;

} UartT;

int GetBat3uData(UartT *UartT, Bat3uResT *res);
esp_err_t InitUart(UartT *pin);
void PrintBat3uData(Bat3uResT *data);
void ZeroData(Bat3uResT *res);
#endif