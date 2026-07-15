#include <I2C.h>
#include <server.h>
#include <string>
#include <wifiSetup.h>
#include <date.h>
#include <time.h>

#define DEV_MODE 0 // Set to 1 for Dev, 0 for Production

#if DEV_MODE
const char *ssid = "Wokwi-GUEST";
const char *password = "";
#else
const char *ssid = "Zyxel_4061";
const char *password = "ip7r3am4f3g8ih7f";
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
const int sda_pin = 13;
const int scl_pin = 14;

bool updateValues = false;

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 5000;

const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600;
const int daylightOffset_sec = 3600;
const int FLASH_LED = 4;
const int RED_LED = 33;

void setup()
{
  Serial.begin(115200);
  Wire.begin(sda_pin, scl_pin);
  setupWifi();
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  setupServer();
  SSEEvents();
  slaveAdress = searchI2C();
  getLocalTime(&timeinfo, 2000);
  Serial.println(&timeinfo);
  pinMode(FLASH_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  digitalWrite(RED_LED, LOW);
  delay(100);
  digitalWrite(RED_LED, HIGH);
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
    receiveI2C();
    lastTime = millis();
  }
}