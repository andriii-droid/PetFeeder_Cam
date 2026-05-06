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
    int top;
} Stack;

Stack dataStack;

// Initialize stack
void initStack(Stack *s)
{
    s->top = -1; // -1 means the stack is empty
}

void push(Stack *s, uint8_t data)
{
    if (s->top == 100 - 1)
    {
        printf("Stack Overflow! Cannot push %d\n", data);
    }
    else
    {
        s->items[++(s->top)] = data;
        printf("Pushed %d onto the stack.\n", data);
    }
}

uint8_t pop(Stack *s)
{
    if (s->top == -1)
    {
        printf("Stack Underflow! The stack is empty.\n");
        return 255; // Error value
    }
    else
    {
        return s->items[(s->top)--];
    }
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

    initStack(&dataStack);

    push(&dataStack, 55);
    push(&dataStack, 0);
    push(&dataStack, 1);
    push(&dataStack, 44);
    push(&dataStack, 0);
    push(&dataStack, 0);
}

bool on_i2c_connect(void *user_data, uint32_t address, bool connect)
{
    return true; /* Ack */
}

uint8_t on_i2c_read(void *user_data) //When Data requested
{
    uint8_t data = pop(&dataStack);
    printf("Sending data from controller: ");
    printf("%x", data);

    return data;
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