#ifndef _BAT3U_H_
#define _BAT3U_H_

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
} Bat3uDataT;


typedef struct
{
    uart_port_t uartNum;
    int txNum, rxNum;

} UartT;

int GetBat3uData(UartT *UartT, Bat3uDataT *res);
esp_err_t InitUart(UartT *pin);
void PrintBat3uData(Bat3uDataT *data);
void ZeroData(Bat3uDataT *res);

extern volatile Bat3uDataT tdsData;
void syncTDS(UartT *pin);
#endif