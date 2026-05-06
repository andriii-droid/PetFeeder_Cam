#include "wokwi-api.h"
#include <stdio.h>
#include <stdlib.h>

const int ADDRESS = 0x22;

typedef struct
{
    pin_t pin_vcc;
    pin_t pin_gnd;
    pin_t pin_scl;
    pin_t pin_sda;
} chip_state_t;

static bool on_i2c_connect(void *user_data, uint32_t address, bool connect);
static uint8_t on_i2c_read(void *user_data);
static bool on_i2c_write(void *user_data, uint8_t data);
static void on_i2c_disconnect(void *user_data);

void chip_init()
{
    chip_state_t *chip = malloc(sizeof(chip_state_t));

    const i2c_config_t i2c_config = {
        .user_data = chip,
        .address = ADDRESS,
        .scl = pin_init("SCL", INPUT_PULLUP),
        .sda = pin_init("SDA", INPUT_PULLUP),
        .connect = on_i2c_connect,
        .read = on_i2c_read,
        .write = on_i2c_write,
        .disconnect = on_i2c_disconnect, // Optional
    };
    i2c_init(&i2c_config);
}

bool on_i2c_connect(void *user_data, uint32_t address, bool connect)
{
    return true; /* Ack */
}

uint8_t on_i2c_read(void *user_data)
{
    printf("Sending byte to controller: ");
    printf("%x\n", 0x2f);
    return 0x2f;
}

bool on_i2c_write(void *user_data, uint8_t data)
{
    printf("Getting data from controller: ");
    printf("%x", data);
    printf("\n");
    return true; // Ack
}

void on_i2c_disconnect(void *user_data)
{
}