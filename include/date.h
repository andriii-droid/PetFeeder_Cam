
#ifndef DATE_H // "If Not Defined"
#define DATE_H

#include <time.h>
#include <Arduino.h>

extern const char *ntpServer;
extern const long gmtOffset_sec;
extern const int daylightOffset_sec;
extern struct tm timeinfo;

String tmToString(tm &time);

void setDate();



#endif // DATE_H