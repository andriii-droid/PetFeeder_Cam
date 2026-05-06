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

void sendI2C(byte id, uint16_t msg)
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
        byte data[3];
        data[0] = id;                
        data[1] = (msg >> 8) & 0xFF; 
        data[2] = msg & 0xFF;

        Wire.beginTransmission(slaveAdress); // Set the slave address
        Wire.write(data, 3);                 // Send the command/data byte
        Wire.endTransmission();

        Serial.print("I2C Send - ID: ");
        Serial.print(id);
        Serial.print(" | New Value: ");
        Serial.println(msg);
    }
    else
    {
        Serial.println("I2C: No data sent");
    }
}

void receiveI2C()
{
    if (slaveAdress != -1)
    {
        while (1)
        {
            byte receivedCount = Wire.requestFrom(slaveAdress, 1);

            if (receivedCount == 1)
            {
                byte id = Wire.read();

                if (id == 255)
                {
                    Serial.println("No Valid Data left");
                    break; // Exit the Loop if no Data left
                }

                // 2. If ID is valid, get the 2 bytes of data
                receivedCount = Wire.requestFrom(slaveAdress, 2);
                if (receivedCount == 2)
                {
                    uint16_t data = (Wire.read() * 10) + Wire.read();

                    Serial.print("I2C Received - ID: ");
                    Serial.print(id);
                    Serial.print(" | Value: ");
                    Serial.println(data);
                    sendEvents(id, data);
                }
            }
            else
            {
                Serial.println("No Answer from Slave");
                break; // Exit if the slave is disconnected
            }
        };
    }
}
