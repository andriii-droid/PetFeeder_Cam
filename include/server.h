#ifndef SERVER_H // "If Not Defined"
#define SERVER_H
#include <ESPAsyncWebServer.h>
#include <map>
#include <html.h>
#include <I2C.h>
#include <date.h>
#include <Arduino.h>

extern AsyncWebServer server;
extern AsyncEventSource events;

// I2C IDs
struct dataStruct
{
    String str;
    byte id;
    uint16_t data;
    byte dataLen;
};
extern dataStruct webData[10];
extern bool updateValues;

void setupServer();

void SSEEvents();

void sendEvents(byte id, uint16_t msg);


String processor(const String &var);

#endif // SERVER_H