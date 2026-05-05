#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

/* Manually defining the Wokwi I2C API to bypass header issues */
typedef enum
{
    I2C_START_READ,
    I2C_START_WRITE,
    I2C_WRITE,
    I2C_READ,
    I2C_STOP,
} i2c_event_t;

typedef bool (*i2c_event_handler_t)(void *user_data, i2c_event_t event, uint8_t data);

typedef struct
{
    uint32_t address;
    uint32_t scl;
    uint32_t sda;
    i2c_event_handler_t event;
    void *user_data;
} i2c_config_t;

// Wokwi API Prototypes
void i2c_init(const i2c_config_t *config);
void i2c_respond(uint32_t sda_pin, uint8_t data);
uint32_t pin_init(const char *name, uint32_t mode);
#define INPUT 0

typedef struct
{
    uint32_t sda_pin;
    uint8_t regs[8];
    uint8_t current_reg;
} chip_state_t;

static bool on_i2c_event(void *user_data, i2c_event_t event, uint8_t data)
{
    chip_state_t *chip = (chip_state_t *)user_data;

    switch (event)
    {
    case I2C_START_WRITE:
    case I2C_START_READ:
        // Returning true here sends an ACK to the master
        return true;

    case I2C_WRITE:
        // The first byte after START_WRITE is usually the register pointer
        chip->current_reg = data % 8;
        return true;

    case I2C_READ:
        i2c_respond(chip->sda_pin, chip->regs[chip->current_reg]);
        return true;

    case I2C_STOP:
        return true;

    default:
        return true;
    }
}

void chip_init()
{
    chip_state_t *chip = malloc(sizeof(chip_state_t));
    for (int i = 0; i < 8; i++)
        chip->regs[i] = i * 10;

    const i2c_config_t i2c_config = {
        .address = 0x42,
        .scl = pin_init("SCL", INPUT),
        .sda = pin_init("SDA", INPUT),
        .event = on_i2c_event,
        .user_data = chip,
    };
    chip->sda_pin = i2c_config.sda;
    i2c_init(&i2c_config);
}