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

typedef struct
{
    uint8_t items[100];
    int head;  // Where we pop from
    int tail;  // Where we push to
    int count; // Current number of items
} Pipeline;

Pipeline dataPipe;

// Initialize stack
void initPipeline(Pipeline *p)
{
    p->head = 0;
    p->tail = 0;
    p->count = 0;
}

bool push(Pipeline *p, uint8_t newData)
{
    if (p->count == 100)
    {
        return false; // Pipeline Full
    }

    p->items[p->tail] = newData;
    p->tail = (p->tail + 1) % 100; // Move tail and wrap if needed
    p->count++;
    return true;
}

uint8_t pop(Pipeline *p)
{
    if (p->count == 0)
    {
        // Return an "Error" struct (all 255s)
        return 255;
    }

    uint8_t out = p->items[p->head];
    p->head = (p->head + 1) % 100; // Move head and wrap
    p->count--;
    return out;
}

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

    initPipeline(&dataPipe);

    push(&dataPipe, 1);
    push(&dataPipe, 0);
    push(&dataPipe, 30);
    push(&dataPipe, 0);
    push(&dataPipe, 0);
    push(&dataPipe, 33);
}

bool on_i2c_connect(void *user_data, uint32_t address, bool connect)
{
    return true; /* Ack */
}

uint8_t on_i2c_read(void *user_data) //When Data requested
{
    uint8_t data = pop(&dataPipe);
    printf("Sending data from controller: ");
    printf("%x", data);

    return data;
}

bool on_i2c_write(void *user_data, uint8_t data)
{
    printf("Getting data from controller: ");
    printf("%x", data);
    printf("\n");
    push(&dataPipe, data);
    return true; // Ack
}

void on_i2c_disconnect(void *user_data)
{
}