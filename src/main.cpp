#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
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
AsyncWebServer server(80);
AsyncEventSource events("/events");
int slaveAdress = -1;

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 30000;

String processor(const String &var)
{
  
  // Serial.println(var);
  if (var == "TEMPERATURE")
  {
    return String(25);
  }
  else if (var == "HUMIDITY")
  {
    return String(34);
  }
  else if (var == "PRESSURE")
  {
    return String(85);
  }
}

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ESP Web Server</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <link rel="icon" href="data:,">
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    p { font-size: 1.2rem;}
    body {  margin: 0;}
    .topnav { overflow: hidden; background-color: #50B8B4; color: white; font-size: 1rem; }
    .content { padding: 20px; }
    .card { background-color: white; box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5); }
    .cards { max-width: 800px; margin: 0 auto; display: grid; grid-gap: 2rem; grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); }
    .reading { font-size: 1.4rem; }
  </style>
</head>
<body>
  <div class="topnav">
    <h1>BME280 WEB SERVER (SSE)</h1>
  </div>
  <div class="content">
    <div class="cards">
      <div class="card">
        <p><i class="fas fa-thermometer-half" style="color:#059e8a;"></i> TEMPERATURE</p><p><span class="reading"><span id="temp">%TEMPERATURE%</span> &deg;C</span></p>
      </div>
      <div class="card">
        <p><i class="fas fa-tint" style="color:#00add6;"></i> HUMIDITY</p><p><span class="reading"><span id="hum">%HUMIDITY%</span> &percnt;</span></p>
      </div>
      <div class="card">
        <p><i class="fas fa-angle-double-down" style="color:#e1e437;"></i> PRESSURE</p><p><span class="reading"><span id="pres">%PRESSURE%</span> hPa</span></p>
      </div>
    </div>
  </div>
<script>
if (!!window.EventSource) {
 var source = new EventSource('/events');
 
 source.addEventListener('open', function(e) {
  console.log("Events Connected");
 }, false);
 source.addEventListener('error', function(e) {
  if (e.target.readyState != EventSource.OPEN) {
    console.log("Events Disconnected");
  }
 }, false);
 
 source.addEventListener('message', function(e) {
  console.log("message", e.data);
 }, false);
 source.addEventListener('temperature', function(e) {
    console.log("temperature", e.data);
    document.getElementById("temp").innerHTML = e.data;
  }, false);

  source.addEventListener('humidity', function(e) {
    console.log("humidity", e.data);
    document.getElementById("hum").innerHTML = e.data;
  }, false);

  source.addEventListener('pressure', function(e) {
    console.log("pressure", e.data);
    document.getElementById("pres").innerHTML = e.data;
  }, false);
 }
</script>
</body>
</html>)rawliteral";

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

  // Handle Web Server
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "text/html", index_html, processor); });

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

  //Scanning for I2C Slave
  slaveAdress = searchI2C();
}

void loop()
{
  if ((millis() - lastTime) > timerDelay)
  {
    Serial.printf("Temperature = %.2f ºC \n", 25);
    Serial.printf("Humidity = %.2f \n", 34);
    Serial.printf("Pressure = %.2f hPa \n", 85);
    Serial.println();

    // Send Events to the Web Client with the Sensor Readings
    events.send("ping", NULL, millis());
    events.send(String(25).c_str(), "temperature", millis());
    events.send(String(34).c_str(), "humidity", millis());
    events.send(String(85).c_str(), "pressure", millis());

    lastTime = millis();
  }
}