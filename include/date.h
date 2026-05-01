
#ifndef DATE_H // "If Not Defined"
#define DATE_H

#include "time.h"
#include <Arduino.h>

extern const char *ntpServer;
extern const long gmtOffset_sec;
extern const int daylightOffset_sec;

void setNewDate();


#endif // DATE_H