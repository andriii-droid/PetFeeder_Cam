#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>

#define DEV_MODE 1 // Set to 1 for Dev, 0 for Production

#if DEV_MODE
const char *ssid = "Wokwi-GUEST";
const char *password = "";
#else
const char *ssid = "";
const char *password = "";
#endif

// Create a web server object
WebServer server(80);

int slaveAdress = -1;

// Function to handle the root URL and show the current states
void handleRoot()
{
  String html = "<!DOCTYPE html><html><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
  html += "<link rel=\"icon\" href=\"data:,\">";
  html += "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}";
  html += ".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px; text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}";
  html += ".button2 { background-color: #555555; }</style></head>";
  html += "<body><h1>ESP32 Web Server</h1>";

  html += "<p>Send I2C command</p>";
  html += "<p><a href=\"/send\"><button class=\"button\">Send</button></a></p>";


  html += "</body></html>";
  server.send(200, "text/html", html);
}

void handleSendI2C() 
{
  if (slaveAdress != -1) {
    Wire.beginTransmission(slaveAdress); // Set the slave address
    Wire.write(0x01);             // Send the command/data byte
    Wire.endTransmission();
    Serial.println("Send 'Hello' over I2C");
  }
  handleRoot();
}

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

void setup()
{
  Serial.begin(115200);
  Wire.begin();

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

  // Set up the web server to handle different routes
  server.on("/", handleRoot);
  server.on("/send", handleSendI2C);

  // Start the web server
  server.begin();
  Serial.println("HTTP server started");

  //Scanning for I2C Slave
  slaveAdress = searchI2C();
}

void loop()
{
  // Handle incoming client requests
  server.handleClient();
}