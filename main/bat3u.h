#ifndef _BAT3U_H_
#define _BAT3U_H_

typedef struct
{
    short TDS;  // 单位 ppm
    short TEMP; // 单位 0.01 摄氏度
} SensorData;

typedef struct
{
    SensorData Sensor1;
    SensorData Sensor2;
    SensorData Sensor3;
} Bat3uData;


typedef struct
{
    uart_port_t uartNum;
    int txNum, rxNum;

} Uart;

int GetBat3uData(Uart *uart, Bat3uData *res);
esp_err_t InitUart(Uart *pin);
void PrintBat3uData(Bat3uData *data);
void ZeroData(Bat3uData *res);
#endif