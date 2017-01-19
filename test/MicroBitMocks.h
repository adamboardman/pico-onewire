#ifndef MICROBITMOCKS_H
#define MICROBITMOCKS_H

#include <cstdint>

class MicroBitSerial {
public:
    int printf(const char *format, ...);

};

class MicroBit {
public:
    // Serial Interface
    MicroBitSerial serial;

};

typedef enum {
    p0  = 0,

    //NORMAL PINS...
    P0_0  = p0,

    // Not connected
    NC = (int) 0xFFFFFFFF
} PinName;


extern std::vector<uint8_t> mockLastCommands;
extern uint8_t mockLastCommand;
extern int mockReadBitPos;
extern size_t mockReadBitsLength;
extern const char *mockReadBits;
extern int writeCount;

class DigitalInOut {

public:
    DigitalInOut(PinName param);

    void output();

    void input();

    void write(int value);

    int read();
};

class DigitalOut {

public:
    DigitalOut(PinName param);

    DigitalOut& operator= (int value);

    void write(int value);
};

void wait_us(int us);
void wait_ms(int ms);

void __disable_irq();
void __enable_irq();

#endif // MICROBITMOCKS_H
