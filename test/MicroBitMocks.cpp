
#include <cstdarg>
#include <cstdio>
#include <vector>
#include "MicroBitMocks.h"

#define STRING_STACK_LIMIT 100

MicroBit uBit;

std::vector<uint8_t> mockLastCommands;
uint8_t mockLastCommand; //Stores the last 8 bits written to the bus
int mockReadBitPos;
size_t mockReadBitsLength;
const char *mockReadBits;
int waitTime;
int writeCount;

DigitalInOut::DigitalInOut(PinName param) {

}

void DigitalInOut::output() {

}

void DigitalInOut::input() {

}

void DigitalInOut::write(int value) {
    if (value == 0) {
        waitTime = 0;
    } else {
        if ((writeCount > 0) && (writeCount % 8 == 0)) {
            mockLastCommands.push_back(mockLastCommand);
            mockLastCommand = 0;
        }
        if (waitTime > 1 && waitTime <= 15) { //Spec for write bit 1
            mockLastCommand >>= 1;
            mockLastCommand |= (1 << 7);
        }
        if (waitTime >= 10 && waitTime <= 120) { //Spec for write bit 0
            mockLastCommand >>= 1;
            mockLastCommand &= ~(1 << 7);
        }
        if (waitTime > 480) { //Spec for reset
            mockLastCommands.push_back(mockLastCommand);
            mockLastCommand = 0;
            writeCount = 0;
        }
        writeCount++;
    }
}

int DigitalInOut::read() {
    int ret = 1;
    if (mockReadBitPos < mockReadBitsLength) {
        if (mockReadBits[mockReadBitPos] == '0') {
            ret = 0;
        }
        mockReadBitPos++;
    } else {
        uBit.serial.printf("Ran out of mock data\n");
    }
    return ret;
}

DigitalOut::DigitalOut(PinName param) {

}

DigitalOut &DigitalOut::operator=(int value) {
    return *this;
}

void DigitalOut::write(int value) {

}

int MicroBitSerial::printf(const char *format, ...) {
    std::va_list arg;
    va_start(arg, format);
    int len = vsnprintf(NULL, 0, format, arg);
    if (len < STRING_STACK_LIMIT) {
        char temp[STRING_STACK_LIMIT];
        vsprintf(temp, format, arg);
        puts(temp);
    } else {
        char *temp = new char[len + 1];
        vsprintf(temp, format, arg);
        puts(temp);
        delete[] temp;
    }
    va_end(arg);
    return len;
}

void wait_us(int us) {
    waitTime += us;
}

void wait_ms(int ms) {
    waitTime += ms * 1000;
}

void __disable_irq() {

}

void __enable_irq() {

}
