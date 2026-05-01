#ifndef WIFISETUP_H
#define WIFISETUP_H

#include <WiFi.h>
#include <Arduino.h>

extern const char *ssid;
extern const char *password;

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

#endif