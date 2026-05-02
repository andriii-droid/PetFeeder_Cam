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
extern std::map<String, byte> I2CID;

void setupServer();

void SSEEvents();

void sendEvents(byte id, byte msg);

String processor(const String &var);

#endif // SERVER_H