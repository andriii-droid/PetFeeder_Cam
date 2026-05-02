#include "server.h"
#include "date.h"
#include <time.h>

struct tm timeinfo;

String tmToString(tm &time)
{
    char buffer[25];
    strftime(buffer, sizeof(buffer), "%d.%m.%y %H:%M:%S", &timeinfo);
    return buffer;
}

void setDate()
{

    events.send(tmToString(timeinfo).c_str(), "lastUpdate", millis());
}