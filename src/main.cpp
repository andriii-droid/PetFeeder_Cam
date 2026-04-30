#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <Wire.h>
#include <html.h>

#define DEV_MODE 1 // Set to 1 for Dev, 0 for Production

#if DEV_MODE
const char *ssid = "Wokwi-GUEST";
const char *password = "";
#else
const char *ssid = "";
const char *password = "";
#endif

// Create a web server object
AsyncWebServer server(80);
AsyncEventSource events("/events");
int slaveAdress = -1;

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 30000;

int searchI2C()
{
  int slaveAddress = -1;
  for (int addr = 1; addr < 127; addr++)
  {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0)
    {
      Serial.print("Found device at 0x");
      Serial.println(addr, HEX);
      slaveAddress = addr;
      break; // Stop at the first device found
    }
  }

  if (slaveAddress == -1)
  {
    Serial.println("No I2C devices found. Check wiring/pull-ups!");
  }

  return slaveAddress;
}

void sendI2C(byte id, byte msg)
{
  if (slaveAdress != -1)
  {
    byte data[2] = {id, msg};
    Wire.beginTransmission(slaveAdress); // Set the slave address
    Wire.write(data, 2);                 // Send the command/data byte
    Wire.endTransmission();
  } else { Serial.println("I2C: No data sent");}
}

void setupWifi()
{
  // Connect to Wi-Fi network
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void getRequests()
{
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "text/html", index_html); });

  server.on("/update", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    String inputID;
    String inputVal;
    
    if (request->hasParam("id") && request->hasParam("val")) {
      inputID = request->getParam("id")->value();
      inputVal = request->getParam("val")->value();
      
      uint8_t byteVal = (uint8_t)constrain(inputVal.toInt(), 0, 255);
      Serial.print("Device: ");
      Serial.print(inputID);
      Serial.print(" - New Value: ");
      Serial.println(byteVal);

      if(inputID == "sMor") {

        sendI2C(1, byteVal);
      }
    }
    request->send(200, "text/plain", "OK"); });
}

void SSEEvents()
{
  // Handle Web Server Events
  events.onConnect([](AsyncEventSourceClient *client)
                   {
    if(client->lastId()){
      Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
    }
    // send event with message "hello!", id current millis
    // and set reconnect delay to 1 second
    client->send("hello!", NULL, millis(), 10000); });
  server.addHandler(&events);
  server.begin();
  Serial.println("Server Started!");
}


void setup()
{
  Serial.begin(115200);
  Wire.begin();
  setupWifi();
  getRequests();
  SSEEvents();
  slaveAdress = searchI2C();
}

void loop()
{
  if ((millis() - lastTime) > timerDelay)
  {
    // Send Events to the Web Client with the Sensor Readings
    events.send(String(millis() % 101).c_str(), "fodderAmount", millis());
    events.send(String(millis() % 101).c_str(), "MorningAmount", millis());
    events.send(String(millis() % 101).c_str(), "NoonAmount", millis());
    events.send(String(millis() % 101).c_str(), "EveningAmount", millis());
    events.send(String(millis() % 2).c_str(), "MorningToggle", millis());
    events.send(String(millis() % 2).c_str(), "NoonToggle", millis());
    events.send(String(millis() % 2).c_str(), "EveningToggle", millis());
    lastTime = millis();
  }
}