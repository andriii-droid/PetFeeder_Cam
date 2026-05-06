#include <I2C.h>
#include <server.h>
#include <string>
#include <wifiSetup.h>
#include <date.h>
#include <time.h>

#define DEV_MODE 1 // Set to 1 for Dev, 0 for Production

#if DEV_MODE
const char *ssid = "Wokwi-GUEST";
const char *password = "";
#else
const char *ssid = "";
const char *password = "";
#endif

AsyncWebServer server(80);
AsyncEventSource events("/events");

dataStruct webData[10] = {
    {"fodderAmount", 0, 0},
    {"sMor", 1, 50, 1},
    {"tMor", 2, 0, 1},
    {"sNoo", 3, 50, 1},
    {"tNoo", 4, 0, 1},
    {"sEve", 5, 50, 1},
    {"tEve", 6, 0, 1},
    {"iMor", 7, 900, 2},
    {"iNoo", 8, 1200, 2},
    {"iEve", 9, 1800, 2}
};

int slaveAdress = -1;

bool updateValues = false;

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 5000;

const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600;
const int daylightOffset_sec = 3600;

void setup()
{
  Serial.begin(115200);
  Wire.begin();
  setupWifi();
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  setupServer();
  SSEEvents();
  slaveAdress = searchI2C();
  getLocalTime(&timeinfo, 2000);
  Serial.println(&timeinfo);
}

void loop()
{
  if (updateValues)
  {
    updateValues = false;
    delay(1000);

    for (auto &entry : webData)
    {
      sendEvents(entry.id, entry.data);
    }
  }
  if ((millis() - lastTime) > timerDelay) //Poll the I2C Slaves every 5 Seconds
  {
    getLocalTime(&timeinfo, 2000);
    if (slaveAdress != -1) {
      byte receivedCount = Wire.requestFrom(slaveAdress, 1); // Holds Exec here until Bytes transmitted from Slave
      if (receivedCount == 1) {
        byte id = Wire.read();
        uint16_t data = 0;

        if (id != -1) {
          receivedCount = Wire.requestFrom(slaveAdress, 2);

          if (receivedCount == 2)
          {
            data = Wire.read() *10;
            data += Wire.read();
          }
          Serial.print("ID: ");
          Serial.println(id);
          Serial.print("Value: ");
          Serial.println(data);
        }
        else (Serial.println("No Valid Data left"));
      } else (Serial.println("No Answer from Slave")); }
    lastTime = millis();
  }
}