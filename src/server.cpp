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

            uint16_t byteVal;
            uint8_t len;
            if (inputVal.length() == 5)
            {
                int hours = inputVal.substring(0, 2).toInt();
                int minutes = inputVal.substring(3, 5).toInt();
                byteVal = (hours * 100) + minutes;
                len = 2;
            } else {
                byteVal = (uint8_t)constrain(inputVal.toInt(), 0, 255);
                len = 1;
            }

            byte id = 0;
            for (auto &entry : webData)
            {
                if (entry.str == inputID)
                {
                    id = entry.id;
                    break;
                }
            }
            sendI2C(id, byteVal);
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

void sendEvents(byte id, uint16_t msg)
{
    String inputID = "";

    for (auto &entry : webData)
    {
        if (entry.id == id)
        {
            inputID = entry.str;
            entry.data = msg;
            break;
        }
    }

    if (inputID != "")
    {
        if (inputID.startsWith("i")) { // If its a time we convert it to a Time string
            int hours = msg / 100;  
            int minutes = msg % 100; 

            String formattedTime = (hours < 10 ? "0" : "") + String(hours) + ":" +
                                   (minutes < 10 ? "0" : "") + String(minutes);
            events.send(formattedTime.c_str(), inputID, millis());
        } else {
            events.send(String(msg).c_str(), inputID, millis());
        }
        setDate();
    }
    // Serial.print("ID: ");
    // Serial.print(inputID);
    // Serial.print(" - Value: ");
    // Serial.println(msg);
}

String processor(const String &var)
{
    updateValues = true;

    if (var == "") {
        return "%";
    } else if (var == "last") {
        return tmToString(timeinfo).c_str();
    }
    else
    {
        return "Error";
    }
}