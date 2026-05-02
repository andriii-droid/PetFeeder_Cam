#include <I2C.h>
#include <server.h>
#include <string>
#include <wifiSetup.h>
// #include <date.h>
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
std::map<String, byte> I2CID = {
    {"fodderAmount", 0},
    {"sMor", 1},
    {"tMor", 2},
    {"sNoo", 3},
    {"tNoo", 4},
    {"sEve", 5},
    {"tEve", 6}};

int slaveAdress = -1;

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
  if ((millis() - lastTime) > timerDelay) //Poll the I2C Slaves every 5 Seconds
  {
    getLocalTime(&timeinfo, 2000);
    if (slaveAdress != -1) {
      byte receivedCount = Wire.requestFrom(slaveAdress, 1); // Holds Exec here until Bytes transmitted from Slave

      if (receivedCount == 1) {
        byte idCount = 0;
         while (Wire.available())
        {
          idCount = Wire.read();
        }
        if (idCount != 0) {
          receivedCount = Wire.requestFrom(slaveAdress, idCount*2);

          if (receivedCount == idCount*2)
          {
            while (Wire.available())
            {
              byte id = Wire.read();
              byte msg = Wire.read();
              sendEvents(id, msg); // Send Event to Webserver
            }
          }
          else
          {
            Serial.println("No Answer from Slave");
          }
        } else 
        {
          Serial.println("No new data to send");
        }
      }
    }
    lastTime = millis();
  }
}