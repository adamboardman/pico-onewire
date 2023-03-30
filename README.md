# Raspberry Pi Pico One Wire Library

This library allows you to talk to one wire devices such as Temperature sensors Dallas DS18S20, DS18B20 and DS1822, Maxim MAX31820 and MAX31826.

It should also be easy to extend it to also support RTC's DS2404 and DS2417, Current measurement devices DS2740 and 1k EEPROM's DS2502.

Based upon Erik Olieman's mbed DS1820 lib

https://developer.mbed.org/components/DS1820/

Modified timings so that it works when used on the Pi Pico

Added off device test code with mocks and its own cmake configuration.

Modified API's for two stage workflow:
1. Listing of devices with addresses to be stored elsewhere
2. Returning temperatures for each device by address

# Usage - Temperature reading

## Include library

Check the module out as a submodule of your project to say: modules/pico-onewire then
add the sub directory and link the library in your CMakeLists.txt:
```
add_subdirectory(modules/pico-onewire)

target_link_libraries( ... pico_one_wire)
```

## Single sensor

An example main that will check for attached devices once per second, print the address and temperature.
```
#include <cstdio>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "modules/pico-onewire/api/one_wire.h"

int main() {
    stdio_init_all();
    One_wire one_wire(15); //GP15 - Pin 20 on Pi Pico
    one_wire.init();
    rom_address_t address{};
    while (true) {
        one_wire.single_device_read_rom(address);
        printf("Device Address: %02x%02x%02x%02x%02x%02x%02x%02x\n", address.rom[0], address.rom[1], address.rom[2], address.rom[3], address.rom[4], address.rom[5], address.rom[6], address.rom[7]);
        one_wire.convert_temperature(address, true, false);
        printf("Temperature: %3.1foC\n", one_wire.temperature(address));
        sleep_ms(1000);
    }
    return 0;
}
```

## Multiple sensors

A more complicated example to print all the available devices.
Additionally pin 22 is used to exit the main loop and return to the USB Flashing mode.
A push button switch between pins 28&29 (GND+GP22) will do nicely.
Also we are converting the address to uint64 for future use.

Add to the CMakeLists.txt in your project config:
```
add_definitions(-DPICO_ENTER_USB_BOOT_ON_EXIT)
```
main.cpp:
```
#include <cstdio>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "modules/pico-onewire/api/one_wire.h"

#define TEMP_SENSE_GPIO_PIN 15
#define EXIT_GPIO_PIN 22

int main() {
	stdio_init_all();
	One_wire one_wire(TEMP_SENSE_GPIO_PIN);
	one_wire.init();
	gpio_init(EXIT_GPIO_PIN);
	gpio_set_dir(EXIT_GPIO_PIN, GPIO_IN);
	gpio_pull_up(EXIT_GPIO_PIN);
	sleep_ms(1);
	while (gpio_get(EXIT_GPIO_PIN)) {
		int count = one_wire.find_and_count_devices_on_bus();
		rom_address_t null_address{};
		one_wire.convert_temperature(null_address, true, true);
		for (int i = 0; i < count; i++) {
			auto address = One_wire::get_address(i);
			printf("%016llX\t%3.1f*C\r\n", One_wire::to_uint64(address), one_wire.temperature(address));
		}
		sleep_ms(1000);
	}
	return 0;
}
```

# Running the test code on a desktop

If your just using the library you don't need to worry about the test code.
If you plan on fixing bugs or extending the library then you must.

Install Catch2 version 3, it might be available on your desktop linux distro,
though possibly only an older version. Or you can download, build and install it:
```
git clone https://github.com/catchorg/Catch2.git
cd Catch2
cmake -Bbuild -H. -DBUILD_TESTING=OFF
cmake --build build/
sudo cmake --build build/ --target install
```

Build and run the test code:
```
cd test
mkdir build
cd build
cmake ..
make
./tests
```

You should then see 'All tests passed'
