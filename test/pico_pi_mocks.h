#ifndef PICO_PI_MOCKS_H
#define PICO_PI_MOCKS_H

#include <cstdint>
#include <vector>

#define GPIO_OUT 1
#define GPIO_IN 0

typedef	unsigned int uint;

extern std::vector<uint8_t> mockLastCommands;
extern uint8_t mockLastCommand;
extern int mockReadBitPos;
extern size_t mockReadBitsLength;
extern const char *mockReadBits;
extern int writeCount;

void sleep_us(int us);

void sleep_ms(int ms);

void gpio_init(uint gpio);

void gpio_set_dir(uint gpio, bool out);

bool gpio_get(uint gpio);

void gpio_put(uint gpio, bool value);

#endif // PICO_PI_MOCKS_H
