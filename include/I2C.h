#ifndef I2C_H // "If Not Defined"
#define I2C_H
#include <Wire.h>
#include <Arduino.h>

// Create a web server object
extern int slaveAdress;

int searchI2C();

void sendI2C(byte id, byte msg);

#endif // I2C_H