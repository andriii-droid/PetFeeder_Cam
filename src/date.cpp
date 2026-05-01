#include <server.h>
#include <date.h>

void setNewDate()
{
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo))
    {
        Serial.println("Failed to obtain time");
        return;
    }
    // Format: YYYY-MM-DD HH:MM:SS
    char buffer[25] = {"error"};
    strftime(buffer, sizeof(buffer), "%d.%m.%y %H:%M:%S", &timeinfo);
    events.send(String(buffer).c_str(), "lastUpdate", millis());
}