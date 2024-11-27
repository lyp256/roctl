#include <stdio.h>
#include <time.h>
#include <string.h>
void timeString(char *dest, time_t t)
{
#define Second 1
#define Minute 60
#define Hour 3600
#define Day 86400
    int d = t / Day;
    t = t % Day;
    int h = t / Hour;
    t = t % Hour;
    int m = t / Minute;
    int s = t % Minute;
    char buf[16];

    if (d != 0)
    {
        strcpy(buf, "");
        sprintf(buf, "%4dd", d);
        strcat(dest, buf);
    }
    if (h != 0 || d != 0)
    {
        strcpy(buf, "");
        sprintf(buf, "%02dh", h);
        strcat(dest, buf);
    }
    if (m != 0 || h != 0 || d != 0)
    {
        strcpy(buf, "");

        sprintf(buf, "%02dm", m);
        strcat(dest, buf);
    }
    strcpy(buf, "");
    sprintf(buf, "%02ds", s);
    strcat(dest, buf);
}

float celsius(short temp)
{
    return ((float)temp) / 100;
}