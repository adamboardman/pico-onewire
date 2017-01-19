#ifndef DS1820TEST_H
#define DS1820TEST_H

#include <cxxtest/TestSuite.h>
#include "MicroBitMocks.h"
#include "OneWire.h"

extern std::vector<rom_address_t> found_addresses;

class OneWireTest : public CxxTest::TestSuite {
public:
    OneWireTest() : oneWire(P0_0) {}

    void setUp() {
    }

    void tearDown() {}

    void testInit() {
        resetLastCommands();
        initialiseModule();

        TS_ASSERT_EQUALS(mockLastCommand, 0xB4);
    }

    void initialiseModule() {
        mockReadBitPos = 0;
        mockReadBits = "01";
        mockReadBitsLength = strlen(mockReadBits);

        oneWire.init();
    }

    void testReadROM() {
        resetLastCommands();
        mockReadBitPos = 0;
        //28 62 24 C7 03 00 00 0F
        mockReadBits = "0"
                "00010100"
                "01000110"
                "00100100"
                "11100011"
                "11000000"
                "00000000"
                "00000000"
                "11110000";
        mockReadBitsLength = strlen(mockReadBits);
        rom_address_t address;
        oneWire.singleDeviceReadROM(address);
        TS_ASSERT_EQUALS(address.rom[0], 0x28);
        TS_ASSERT_EQUALS(address.rom[1], 0x62);
        TS_ASSERT_EQUALS(address.rom[2], 0x24);
        TS_ASSERT_EQUALS(address.rom[3], 0xC7);
        TS_ASSERT_EQUALS(address.rom[4], 0x03);
        TS_ASSERT_EQUALS(address.rom[5], 0x00);
        TS_ASSERT_EQUALS(address.rom[6], 0x00);
        TS_ASSERT_EQUALS(address.rom[7], 0x0F);
        TS_ASSERT_EQUALS(mockLastCommand, 0x33);
    }

    void resetLastCommands() {
        mockLastCommand = 0;
        mockLastCommands.clear();
        writeCount = 0;
    }

    void testConvertTemperature() {
        initialiseModule();
        resetLastCommands();
        mockReadBitPos = 0;
        mockReadBits = "0";
        mockReadBitsLength = strlen(mockReadBits);
        rom_address_t address;
        oneWire.convertTemperature(address, false, true);
        TS_ASSERT_EQUALS(mockLastCommand, 0x44);//Convert T
        TS_ASSERT_EQUALS(mockLastCommands[0], 0xCC);//Skip ROM
    }

    void testReadTemperature() {
        resetLastCommands();
        mockReadBitPos = 0;
        mockReadBits = "0"
                "10100000" //0x05
                "10000000" //0x01
                "11010010" //0x4B
                "01100010" //0x46
                "11111110" //0x7F
                "11111111" //0xFF
                "11010000" //0x0B
                "00001000" //0x10
                "10110011" //0xCD
                ;
        mockReadBitsLength = strlen(mockReadBits);

        rom_address_t address = OneWire::addressFromHex("286224C70300000F");
        float temperature = oneWire.temperature(address);
        TS_ASSERT_EQUALS(temperature, 16.3125);
        TS_ASSERT_EQUALS(mockLastCommand, 0xBE);//Read Scratchpad
        TS_ASSERT_EQUALS(mockLastCommands[0], 0x55);//Match ROM
        TS_ASSERT_EQUALS(mockLastCommands.at(1), 0x28);//ROM
        TS_ASSERT_EQUALS(mockLastCommands.at(2), 0x62);//ROM
        TS_ASSERT_EQUALS(mockLastCommands.at(3), 0x24);//ROM
        TS_ASSERT_EQUALS(mockLastCommands.at(4), 0xC7);//ROM
        TS_ASSERT_EQUALS(mockLastCommands.at(5), 0x03);//ROM
        TS_ASSERT_EQUALS(mockLastCommands.at(6), 0x00);//ROM
        TS_ASSERT_EQUALS(mockLastCommands.at(7), 0x00);//ROM
        TS_ASSERT_EQUALS(mockLastCommands.at(8), 0x0F);//ROM
    }

    void testReadTemperature125() {
        resetLastCommands();
        mockReadBitPos = 0;
        mockReadBits = "0"
                "00001011" //0xD0
                "11100000" //0x07
                "11010010" //0x4B
                "01100010" //0x46
                "11111110" //0x7F
                "11111111" //0xFF
                "11010000" //0x0B
                "00001000" //0x10
                "01011001" //0x9A
                ;
        mockReadBitsLength = strlen(mockReadBits);

        rom_address_t address = OneWire::addressFromHex("286224C70300000F");
        float temperature = oneWire.temperature(address, 0);
        TS_ASSERT_EQUALS(temperature, 125.0);
        TS_ASSERT_EQUALS(mockLastCommand, 0xBE);//Read Scratchpad
        TS_ASSERT_EQUALS(mockLastCommands.at(0), 0x55);//Match ROM
        TS_ASSERT_EQUALS(mockLastCommands.at(1), 0x28);//ROM
        TS_ASSERT_EQUALS(mockLastCommands.at(2), 0x62);//ROM
        TS_ASSERT_EQUALS(mockLastCommands.at(3), 0x24);//ROM
        TS_ASSERT_EQUALS(mockLastCommands.at(4), 0xC7);//ROM
        TS_ASSERT_EQUALS(mockLastCommands.at(5), 0x03);//ROM
        TS_ASSERT_EQUALS(mockLastCommands.at(6), 0x00);//ROM
        TS_ASSERT_EQUALS(mockLastCommands.at(7), 0x00);//ROM
        TS_ASSERT_EQUALS(mockLastCommands.at(8), 0x0F);//ROM
    }

    void testSearchROMSingleDevice() {
        /**
         * Search ROM command causes all 1-wire devices to respond simultaneously with their addresses
         * as pairs of bits for each bit of the address, first the true bit then the inversion.
         * As each bit is read back if all devices have the same value we just move to the next bit.
         * When there is a difference each device will hold the line down at different times to indicate
         * a zero bit for the true case (device A) and for the inversion case (device B).
         * As we are reading each bit of the address we are also writing them back to the bus, only
         * devices that match that address keep responding, so each time we have conflicting devices
         * we pick one and carry on.
         */

        resetLastCommands();
        mockReadBitPos = 0;
        mockReadBits = "0"
                "0101011001100101"
                "0110010101101001"
                "0101100101100101"
                "1010100101011010"
                "1010010101010101"
                "0101010101010101"
                "0101010101010101"
                "1010101001010101"
                "0"
                "0101011001100101"
                "0110010101101001"
                "0101100101100101"
                "1010100101011010"
                "1010010101010101"
                "0101010101010101"
                "0101010101010101"
                "1010101001010101";
        mockReadBitsLength = strlen(mockReadBits);
        int found = oneWire.findAllDevicesOnBus();
        TS_ASSERT_EQUALS(found, 1);
        TS_ASSERT_EQUALS(mockLastCommands.at(0), 0xF0);//Search ROM
        TS_ASSERT_EQUALS(mockLastCommands.at(1), 0x28);//ROM
        TS_ASSERT_EQUALS(mockLastCommands.at(2), 0x62);//ROM
        TS_ASSERT_EQUALS(mockLastCommands.at(3), 0x24);//ROM
        TS_ASSERT_EQUALS(mockLastCommands.at(4), 0xC7);//ROM
        TS_ASSERT_EQUALS(mockLastCommands.at(5), 0x03);//ROM
        TS_ASSERT_EQUALS(mockLastCommands.at(6), 0x00);//ROM
        TS_ASSERT_EQUALS(mockLastCommands.at(7), 0x00);//ROM
        TS_ASSERT_EQUALS(mockLastCommand, 0x0F);//ROM

        rom_address_t ROM_address = oneWire.getAddress(0);
        TS_ASSERT_EQUALS(ROM_address.rom[0], 0x28);
        TS_ASSERT_EQUALS(ROM_address.rom[1], 0x62);
        TS_ASSERT_EQUALS(ROM_address.rom[2], 0x24);
        TS_ASSERT_EQUALS(ROM_address.rom[3], 0xC7);
        TS_ASSERT_EQUALS(ROM_address.rom[4], 0x03);
        TS_ASSERT_EQUALS(ROM_address.rom[5], 0x00);
        TS_ASSERT_EQUALS(ROM_address.rom[6], 0x00);
        TS_ASSERT_EQUALS(ROM_address.rom[7], 0x0F);
    }

    void testSearchROMSecondDevice() {
        found_addresses.clear();
        resetLastCommands();
        mockReadBitPos = 0;
        //28 08 81 FB 07 00 00 26 - first
        //28 62 24 C7 03 00 00 0F - second
        mockReadBits = "0"
                "0101011001100101"
                "0100011001010101"// Second bit is first difference, indicated with double zero's
                "1001010101010110"
                "1010011010101010"
                "1010100101010101"
                "0101010101010101"
                "0101010101010101"
                "0110100101100101"
                "0"
                "0101011001100101"
                "0100011001010101"// Second bit is first difference, indicated with double zero's
                "1001010101010110"
                "1010011010101010"
                "1010100101010101"
                "0101010101010101"
                "0101010101010101"
                "0110100101100101"
                "0"
                "0101011001100101"
                "0100010101101001"// Second bit is first difference, indicated with double zero's
                "0101100101100101"
                "1010100101011010"
                "1010010101010101"
                "0101010101010101"
                "0101010101010101"
                "1010101001010101"
                "0"
                "0101011001100101"
                "0110010101101001"
                "0101100101100101"
                "1010100101011010"
                "1010010101010101"
                "0101010101010101"
                "0101010101010101"
                "1010101001010101";
        mockReadBitsLength = strlen(mockReadBits);

        int found = oneWire.findAllDevicesOnBus();
        TS_ASSERT_EQUALS(found, 2);
        TS_ASSERT_EQUALS(mockLastCommands.at(0), 0xF0);//Search ROM
        TS_ASSERT_EQUALS(mockLastCommands.at(1), 0x28);//ROM
        TS_ASSERT_EQUALS(mockLastCommands.at(2), 0x08);//ROM
        TS_ASSERT_EQUALS(mockLastCommands.at(3), 0x81);//ROM
        TS_ASSERT_EQUALS(mockLastCommands.at(4), 0xFB);//ROM
        TS_ASSERT_EQUALS(mockLastCommands.at(5), 0x07);//ROM
        TS_ASSERT_EQUALS(mockLastCommands.at(6), 0x00);//ROM
        TS_ASSERT_EQUALS(mockLastCommands.at(7), 0x00);//ROM
        TS_ASSERT_EQUALS(mockLastCommands.at(8), 0x26);//ROM
        TS_ASSERT_EQUALS(mockLastCommands.at(9), 0xF0);//Search ROM
        TS_ASSERT_EQUALS(mockLastCommands.at(10), 0x28);//ROM
        TS_ASSERT_EQUALS(mockLastCommands.at(11), 0x08);//ROM
        TS_ASSERT_EQUALS(mockLastCommands.at(12), 0x81);//ROM
        TS_ASSERT_EQUALS(mockLastCommands.at(13), 0xFB);//ROM
        TS_ASSERT_EQUALS(mockLastCommands.at(14), 0x07);//ROM
        TS_ASSERT_EQUALS(mockLastCommands.at(15), 0x00);//ROM
        TS_ASSERT_EQUALS(mockLastCommands.at(16), 0x00);//ROM
        TS_ASSERT_EQUALS(mockLastCommands.at(17), 0x26);//ROM
        TS_ASSERT_EQUALS(mockLastCommands.at(18), 0xF0);//Search ROM
        TS_ASSERT_EQUALS(mockLastCommands.at(19), 0x28);//ROM
        TS_ASSERT_EQUALS(mockLastCommands.at(20), 0x62);//ROM
        TS_ASSERT_EQUALS(mockLastCommands.at(21), 0x24);//ROM
        TS_ASSERT_EQUALS(mockLastCommands.at(22), 0xC7);//ROM
        TS_ASSERT_EQUALS(mockLastCommands.at(23), 0x03);//ROM
        TS_ASSERT_EQUALS(mockLastCommands.at(24), 0x00);//ROM
        TS_ASSERT_EQUALS(mockLastCommands.at(25), 0x00);//ROM
        TS_ASSERT_EQUALS(mockLastCommand, 0x0F);//ROM

        rom_address_t ROM_address = oneWire.getAddress(0);
        TS_ASSERT_EQUALS(ROM_address.rom[0], 0x28);
        TS_ASSERT_EQUALS(ROM_address.rom[1], 0x08);
        TS_ASSERT_EQUALS(ROM_address.rom[2], 0x81);
        TS_ASSERT_EQUALS(ROM_address.rom[3], 0xFB);
        TS_ASSERT_EQUALS(ROM_address.rom[4], 0x07);
        TS_ASSERT_EQUALS(ROM_address.rom[5], 0x00);
        TS_ASSERT_EQUALS(ROM_address.rom[6], 0x00);
        TS_ASSERT_EQUALS(ROM_address.rom[7], 0x26);

        ROM_address = oneWire.getAddress(1);
        TS_ASSERT_EQUALS(ROM_address.rom[0], 0x28);
        TS_ASSERT_EQUALS(ROM_address.rom[1], 0x62);
        TS_ASSERT_EQUALS(ROM_address.rom[2], 0x24);
        TS_ASSERT_EQUALS(ROM_address.rom[3], 0xC7);
        TS_ASSERT_EQUALS(ROM_address.rom[4], 0x03);
        TS_ASSERT_EQUALS(ROM_address.rom[5], 0x00);
        TS_ASSERT_EQUALS(ROM_address.rom[6], 0x00);
        TS_ASSERT_EQUALS(ROM_address.rom[7], 0x0F);

    }


    void testListAllAttachedDevices() {
        found_addresses.clear();
        resetLastCommands();
        mockReadBitPos = 0;
        //28 08 81 FB 07 00 00 26 - first
        //28 62 24 C7 03 00 00 0F - second
        mockReadBits = "0"
                "0101011001100101"
                "0100011001010101"// Second bit is first difference, indicated with double zero's
                "1001010101010110"
                "1010011010101010"
                "1010100101010101"
                "0101010101010101"
                "0101010101010101"
                "0110100101100101"
                "0"
                "0101011001100101"
                "0100011001010101"// Second bit is first difference, indicated with double zero's
                "1001010101010110"
                "1010011010101010"
                "1010100101010101"
                "0101010101010101"
                "0101010101010101"
                "0110100101100101"
                "0"
                "0101011001100101"
                "0100010101101001"// Second bit is first difference, indicated with double zero's
                "0101100101100101"
                "1010100101011010"
                "1010010101010101"
                "0101010101010101"
                "0101010101010101"
                "1010101001010101"
                "0"
                "0101011001100101"
                "0110010101101001"
                "0101100101100101"
                "1010100101011010"
                "1010010101010101"
                "0101010101010101"
                "0101010101010101"
                "1010101001010101";
        mockReadBitsLength = strlen(mockReadBits);

        oneWire.findAllDevicesOnBus();

        rom_address_t ROM_address = oneWire.getAddress(0);
        TS_ASSERT_EQUALS(ROM_address.rom[0], 0x28);
        TS_ASSERT_EQUALS(ROM_address.rom[1], 0x08);
        TS_ASSERT_EQUALS(ROM_address.rom[2], 0x81);
        TS_ASSERT_EQUALS(ROM_address.rom[3], 0xFB);
        TS_ASSERT_EQUALS(ROM_address.rom[4], 0x07);
        TS_ASSERT_EQUALS(ROM_address.rom[5], 0x00);
        TS_ASSERT_EQUALS(ROM_address.rom[6], 0x00);
        TS_ASSERT_EQUALS(ROM_address.rom[7], 0x26);

        ROM_address = oneWire.getAddress(1);
        TS_ASSERT_EQUALS(ROM_address.rom[0], 0x28);
        TS_ASSERT_EQUALS(ROM_address.rom[1], 0x62);
        TS_ASSERT_EQUALS(ROM_address.rom[2], 0x24);
        TS_ASSERT_EQUALS(ROM_address.rom[3], 0xC7);
        TS_ASSERT_EQUALS(ROM_address.rom[4], 0x03);
        TS_ASSERT_EQUALS(ROM_address.rom[5], 0x00);
        TS_ASSERT_EQUALS(ROM_address.rom[6], 0x00);
        TS_ASSERT_EQUALS(ROM_address.rom[7], 0x0F);

    }

private:
    OneWire oneWire;
};


#endif // DS1820TEST_H
