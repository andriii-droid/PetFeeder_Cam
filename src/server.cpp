#include <server.h>

void setupServer()
{
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              {
    request->send(200, "text/html", String(index_html), processor); });

    server.on("/update", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        String inputID;
        String inputVal;
        
        if (request->hasParam("id") && request->hasParam("val")) {
        inputID = request->getParam("id")->value();
        inputVal = request->getParam("val")->value();
        
        uint8_t byteVal = (uint8_t)constrain(inputVal.toInt(), 0, 255);

        byte id = I2CID[inputID];
        sendI2C(id, byteVal);

        Serial.print("ID: ");
        Serial.print(id);
        Serial.print(" - New Value: ");
        Serial.println(byteVal);
        }
        request->send(200, "text/plain", "OK"); 
    });
}

void SSEEvents()
{
    // Handle Web Server Events
    events.onConnect([](AsyncEventSourceClient *client)
                     {
    if(client->lastId()){
      Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
    }
    // and set reconnect delay to 1 second
    client->send("hello!", NULL, millis(), 10000); });
    server.addHandler(&events);
    server.begin();
    Serial.println("Server Started!");
}

void sendEvents(byte id, byte msg)
{
    String inputID = "";

    for (auto const &entry : I2CID)
    {
        if (entry.second == id)
        {
            inputID = entry.first;
            break;
        }
    }

    if (inputID != "")
    {
        events.send(String(msg).c_str(), inputID, millis());
        setNewDate();
    }
    Serial.println("Event sent");
}

String processor(const String &var)
{
    if (var == "") {
        return "%";
    } else if (var == "last")
    {
        return String(0); // Whatever variable holds your timestamp
    }
}