#include <I2C.h>
#include <server.h>

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

void sendI2C(byte id, byte msg)
{
    for (auto &entry : webData)
    {
        if (entry.id == id)
        {
            entry.data = msg;
            break;
        }
    }

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