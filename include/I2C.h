#ifndef I2C_H // "If Not Defined"
#define I2C_H
#include <Wire.h>

// Create a web server object
extern int slaveAdress;

inline int searchI2C()
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

inline void sendI2C(byte id, byte msg)
{
    if (slaveAdress != -1)
    {
        byte data[2] = {id, msg};
        Wire.beginTransmission(slaveAdress); // Set the slave address
        Wire.write(data, 2);                 // Send the command/data byte
        Wire.endTransmission();
    }
    else
    {
        Serial.println("I2C: No data sent");
    }
}

#endif // I2C_H