/*
 * micro:bit OneWire Library, derived from the mbed DS1820 Library, for the
 * Dallas (Maxim) 1-Wire Digital Thermometer
 * Copyright (c) 2010, Michael Hagberg Michael@RedBoxCode.com
 *
 * This version uses a single instance to talk to multiple one wire devices.
 * During configuration the devices will be listed and the addresses
 * then stored within the system  they are associated with.
 *
 * Then previously stored addresses are used to query devices.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef MICROBIT_ONEWIRE_H
#define MICROBIT_ONEWIRE_H

#ifdef MOCK_MICROBIT

#include "MicroBitMocks.h"

#else

#include "MicroBit.h"

#endif

#define FAMILY_CODE address.rom[0]
#define FAMILY_CODE_DS18S20 0x10 //9bit temp
#define FAMILY_CODE_DS18B20 0x28 //9-12bit temp also known as MAX31820
#define FAMILY_CODE_DS1822  0x22 //9-12bit temp
#define FAMILY_CODE_MAX31826 0x3B //12bit temp + 1k EEPROM
#define FAMILY_CODE_DS2404 0x04 //RTC
#define FAMILY_CODE_DS2417 0x27 //RTC
#define FAMILY_CODE_DS2740 0x36 //Current measurement
#define FAMILY_CODE_DS2502 0x09 //1k EEPROM

static const int ReadScratchPadCommand = 0xBE;
static const int ReadPowerSupplyCommand = 0xB4;
static const int ConvertTempCommand = 0x44;
static const int MatchROMCommand = 0x55;
static const int ReadROMCommand = 0x33;
static const int SearchROMCommand = 0xF0;
static const int SkipROMCommand = 0xCC;
static const int WriteScratchPadCommand = 0x4E;
struct rom_address_t {
    uint8_t rom[8];
};

/**
 * OneWire with DS1820 Dallas 1-Wire Temperature Probe
 *
 * Example:
 * @code
 * #include "OneWire.h"
 *
 * OneWire oneWire(uBit.io.P12.name);
 *  
 * int main() {
 *     oneWire.init();
 *     rom_address_t address;
       oneWire.singleDeviceReadROM(address);
 *     while(1) {
 *         oneWire.convertTemperature(address, true, true); //Start temperature conversion, wait until ready
 *         printf("It is %3.1foC\r\n", oneWire.temperature(address));
 *         wait(1);
 *     }
 * }
 * @endcode
 */
class OneWire {
public:

    enum {
        invalid_conversion = -1000
    };

    /** Create a one wire bus object connected to the specified pins
     *
     * The bus might either by regular powered or parasite powered. If it is parasite
     * powered and power_pin is set, that pin will be used to switch an external mosfet
     * connecting data to Vdd. If it is parasite powered and the pin is not set, the
     * regular data pin is used to supply extra power when required. This will be
     * sufficient as long as the number of devices is limited.
     *
     * @param data_pin DigitalInOut pin for the data bus
     * @param power_pin DigitalOut (optional) pin to control the power MOSFET
     * @param power_polarity bool (optional) which sets active state (0 for active low (default), 1 for active high)
     */
    OneWire(PinName data_pin, PinName power_pin = NC, bool power_polarity = 0); // Constructor with parasite power pin
    ~OneWire();

    /**
     * Initialise and determine if any devices are using parasitic power
     */
    void init();

    /**
     * Finds all one wire devices and returns the count
     *
     * @return - number of devices found
     */
    int findAllDevicesOnBus();

    /**
     * Get address of devices previously found
     *
     * @param index the index into found devices
     * @return the address of
     */
    rom_address_t &getAddress(int index);

    /** This routine will initiate the temperature conversion within
      * one or all temperature devices.
      *
      * @param wait if true or parasitic power is used, waits up to 750 ms for
      * conversion otherwise returns immediately.
      * @param address allows the function to apply to a specific device or
      * to all devices on the 1-Wire bus.
      * @returns milliseconds until conversion will complete.
      */
    int convertTemperature(rom_address_t &address, bool wait, bool all);

    /** This function will return the temperature measured by the specific device.
      *
      * @param convertToFarenheight whether to convert the degC to farenheight
      * @returns temperature for that scale, or OneWire::invalid_conversion (-1000) if CRC error detected.
      */
    float temperature(rom_address_t &address, bool convertToFarenheight = false);

    /** This function sets the temperature resolution for supported devices
      * in the configuration register.
      *
      * @param a number between 9 and 12 to specify resolution
      * @returns true if successful
      */
    bool setResolution(rom_address_t &address, unsigned int resolution);

    /**
     * Assuming a single device is attached, do a Read ROM
     *
     * @param ROM_address the address will be filled into this parameter
     */
    void singleDeviceReadROM(rom_address_t &ROM_address);

    /**
     * Static utility method for easy conversion from previously stored addresses
     *
     * @param hexAddress the address as a human readable hex string
     * @return
     */
    static rom_address_t addressFromHex(const char *hexAddress);

private:
    DigitalInOut _datapin;
    DigitalOut _parasitepin;
    bool _parasite_power;
    bool _power_mosfet;
    bool _power_polarity;
    uint8_t RAM[9];

    uint8_t CRC_byte(uint8_t _CRC, uint8_t byte);

    void bitWrite(uint8_t &value, int bit, bool set);

    bool onewire_reset();

    void match_ROM(rom_address_t &address);

    void skip_ROM();

    void onewire_bit_out(bool bit_data);

    void onewire_byte_out(uint8_t data);

    bool onewire_bit_in();

    uint8_t onewire_byte_in();

    bool ROM_checksum_error(uint8_t *_ROM_address);

    bool RAM_checksum_error();

    bool searchRomFindNext();

    void readScratchPad(rom_address_t &address);

    void writeScratchPad(rom_address_t &address, int data);

    bool powerSupplyAvailable(rom_address_t &address, bool all);

};


#endif // MICROBIT_ONEWIRE_H