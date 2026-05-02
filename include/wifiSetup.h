#ifndef WIFISETUP_H
#define WIFISETUP_H

#include <WiFi.h>
#include <Arduino.h>

extern const char *ssid;
extern const char *password;

void setupWifi();

#endif