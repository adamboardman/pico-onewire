#include <catch2/catch_test_macros.hpp>
#include <cstdarg>
#include <cstdio>
#include <vector>

#include "pico_pi_mocks.h"

#define STRING_STACK_LIMIT 100

std::vector<uint8_t> mockLastCommands;
uint8_t mockLastCommand;//Stores the last 8 bits written to the bus
int mockReadBitPos;
size_t mockReadBitsLength;
const char *mockReadBits;
int waitTime;
int writeCount;
bool gpio_out_direction[30];
bool gpio_initialised[30]{false};

void gpio_init(uint gpio) {
	gpio_initialised[gpio] = true;
}

void gpio_put(uint gpio, bool value) {
	REQUIRE(gpio_initialised[gpio] == true);
	REQUIRE(gpio_out_direction[gpio] == true);
	if (value == 0) {
		waitTime = 0;
	} else {
		if ((writeCount > 0) && (writeCount % 8 == 0)) {
			mockLastCommands.push_back(mockLastCommand);
			mockLastCommand = 0;
		}
		if (waitTime > 1 && waitTime <= 15) {//Spec for write bit 1
			mockLastCommand >>= 1;
			mockLastCommand |= (1 << 7);
		}
		if (waitTime >= 10 && waitTime <= 120) {//Spec for write bit 0
			mockLastCommand >>= 1;
			mockLastCommand &= ~(1 << 7);
		}
		if (waitTime > 480) {//Spec for reset
			mockLastCommands.push_back(mockLastCommand);
			mockLastCommand = 0;
			writeCount = 0;
		}
		writeCount++;
	}
}

bool gpio_get(uint gpio) {
	int ret = 1;
	REQUIRE(gpio_initialised[gpio] == true);
	REQUIRE(gpio_out_direction[gpio] == false);
	if (mockReadBitPos < mockReadBitsLength) {
		if (mockReadBits[mockReadBitPos] == '0') {
			ret = 0;
		}
		mockReadBitPos++;
	} else {
		printf("Ran out of mock data\n");
	}
	return ret;
}

void gpio_set_dir(uint gpio, bool out) {
	REQUIRE(gpio_initialised[gpio] == true);
	gpio_out_direction[gpio] = out;
}

void sleep_us(int us) {
	waitTime += us;
}

void sleep_ms(int ms) {
	waitTime += ms * 1000;
}
