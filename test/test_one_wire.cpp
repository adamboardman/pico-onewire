#include <catch2/catch_test_macros.hpp>
#include <cstdarg>
#include <cstring>
#include <vector>

#include "one_wire.h"

extern std::vector<rom_address_t> found_addresses;

One_wire one_wire(0); //NOLINT

void resetLastCommands() {
	mockLastCommand = 0;
	mockLastCommands.clear();
	writeCount = 0;
}

void initialiseModule() {
	mockReadBitPos = 0;
	mockReadBits = "01";
	mockReadBitsLength = strlen(mockReadBits);

	one_wire.init();
}

void test_resolution(unsigned int resolution, uint8_t expected) {
	initialiseModule();
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

	rom_address_t address = One_wire::address_from_hex("286224C70300000F");
	one_wire.set_resolution(address, resolution);

	REQUIRE(mockLastCommands[0] == MatchROMCommand);
	REQUIRE(mockLastCommands[1] == 0x28);//address
	REQUIRE(mockLastCommands[2] == 0x62);//address
	REQUIRE(mockLastCommands[3] == 0x24);//address
	REQUIRE(mockLastCommands[4] == 0xC7);//address
	REQUIRE(mockLastCommands[5] == 0x03);//address
	REQUIRE(mockLastCommands[6] == 0x00);//address
	REQUIRE(mockLastCommands[7] == 0x00);//address
	REQUIRE(mockLastCommands[8] == 0x0F);//address
	REQUIRE(mockLastCommands[9] == WriteScratchPadCommand);
	REQUIRE(mockLastCommands[10] == 0x0);
	REQUIRE(mockLastCommands[11] == 0x0);
	REQUIRE(mockLastCommand == expected);
}

TEST_CASE("Init", "[one_wire]") {
	resetLastCommands();
	initialiseModule();

	REQUIRE(mockLastCommand == ReadPowerSupplyCommand);
}

TEST_CASE("Set Resolution 9", "[one_wire]") {
	test_resolution(9, 0x0);
}

TEST_CASE("Set Resolution 10", "[one_wire]") {
	test_resolution(10, 0x20);
}

TEST_CASE("Set Resolution 11", "[one_wire]") {
	test_resolution(11, 0x40);
}

TEST_CASE("Set Resolution 12", "[one_wire]") {
	test_resolution(12, 0x60);
}

TEST_CASE("Read ROM", "[one_wire]") {
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
	rom_address_t address{};
	one_wire.single_device_read_rom(address);
	REQUIRE(address.rom[0] == 0x28);
	REQUIRE(address.rom[1] == 0x62);
	REQUIRE(address.rom[2] == 0x24);
	REQUIRE(address.rom[3] == 0xC7);
	REQUIRE(address.rom[4] == 0x03);
	REQUIRE(address.rom[5] == 0x00);
	REQUIRE(address.rom[6] == 0x00);
	REQUIRE(address.rom[7] == 0x0F);
	REQUIRE(mockLastCommand == ReadROMCommand);
}

TEST_CASE("Convert Temperature", "[one_wire]") {
	initialiseModule();
	resetLastCommands();
	mockReadBitPos = 0;
	mockReadBits = "0";
	mockReadBitsLength = strlen(mockReadBits);
	rom_address_t address{};
	one_wire.convert_temperature(address, false, true);
	REQUIRE(mockLastCommand == ConvertTempCommand);
	REQUIRE(mockLastCommands[0] == SkipROMCommand);
}

TEST_CASE("ReadTemperature", "[one_wire]") {
	initialiseModule();
	resetLastCommands();
	mockReadBitPos = 0;
	mockReadBits = "0"
				   "10100000"//0x05
				   "10000000"//0x01
				   "11010010"//0x4B
				   "01100010"//0x46
				   "11111110"//0x7F
				   "11111111"//0xFF
				   "11010000"//0x0B
				   "00001000"//0x10
				   "10110011"//0xCD
			;
	mockReadBitsLength = strlen(mockReadBits);

	rom_address_t address = One_wire::address_from_hex("286224C70300000F");
	float temperature = one_wire.temperature(address);
	REQUIRE(temperature == 16.3125);
	REQUIRE(mockLastCommand == ReadScratchPadCommand);
	REQUIRE(mockLastCommands[0] == MatchROMCommand);
	REQUIRE(mockLastCommands[1] == 0x28);
	REQUIRE(mockLastCommands[2] == 0x62);
	REQUIRE(mockLastCommands[3] == 0x24);
	REQUIRE(mockLastCommands[4] == 0xC7);
	REQUIRE(mockLastCommands[5] == 0x03);
	REQUIRE(mockLastCommands[6] == 0x00);
	REQUIRE(mockLastCommands[7] == 0x00);
	REQUIRE(mockLastCommands[8] == 0x0F);
}

TEST_CASE("ReadTemperature125", "[one_wire]") {
	initialiseModule();
	resetLastCommands();
	mockReadBitPos = 0;
	mockReadBits = "0"
				   "00001011"//0xD0
				   "11100000"//0x07
				   "11010010"//0x4B
				   "01100010"//0x46
				   "11111110"//0x7F
				   "11111111"//0xFF
				   "11010000"//0x0B
				   "00001000"//0x10
				   "01011001"//0x9A
			;
	mockReadBitsLength = strlen(mockReadBits);

	rom_address_t address = One_wire::address_from_hex("286224C70300000F");
	float temperature = one_wire.temperature(address, false);
	REQUIRE(temperature == 125.0);
	REQUIRE(mockLastCommands[0] == MatchROMCommand);
	REQUIRE(mockLastCommands[1] == 0x28);
	REQUIRE(mockLastCommands[2] == 0x62);
	REQUIRE(mockLastCommands[3] == 0x24);
	REQUIRE(mockLastCommands[4] == 0xC7);
	REQUIRE(mockLastCommands[5] == 0x03);
	REQUIRE(mockLastCommands[6] == 0x00);
	REQUIRE(mockLastCommands[7] == 0x00);
	REQUIRE(mockLastCommands[8] == 0x0F);
	REQUIRE(mockLastCommand == ReadScratchPadCommand);
}

TEST_CASE("SearchROMSingleDevice", "[one_wire]") {
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
	found_addresses.clear();
	initialiseModule();
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
	int found = one_wire.find_and_count_devices_on_bus();
	REQUIRE(found == 1);
	REQUIRE(mockLastCommands[0] == SearchROMCommand);
	REQUIRE(mockLastCommands[1] == 0x28);
	REQUIRE(mockLastCommands[2] == 0x62);
	REQUIRE(mockLastCommands[3] == 0x24);
	REQUIRE(mockLastCommands[4] == 0xC7);
	REQUIRE(mockLastCommands[5] == 0x03);
	REQUIRE(mockLastCommands[6] == 0x00);
	REQUIRE(mockLastCommands[7] == 0x00);
	REQUIRE(mockLastCommand == 0x0F);

	rom_address_t ROM_address = One_wire::get_address(0);
	REQUIRE(ROM_address.rom[0] == 0x28);
	REQUIRE(ROM_address.rom[1] == 0x62);
	REQUIRE(ROM_address.rom[2] == 0x24);
	REQUIRE(ROM_address.rom[3] == 0xC7);
	REQUIRE(ROM_address.rom[4] == 0x03);
	REQUIRE(ROM_address.rom[5] == 0x00);
	REQUIRE(ROM_address.rom[6] == 0x00);
	REQUIRE(ROM_address.rom[7] == 0x0F);
}

TEST_CASE("SearchROMSecondDevice", "[one_wire]") {
	found_addresses.clear();
	initialiseModule();
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

	int found = one_wire.find_and_count_devices_on_bus();
	REQUIRE(found == 2);
	REQUIRE(mockLastCommands[0] == SearchROMCommand);
	REQUIRE(mockLastCommands[1] == 0x28);
	REQUIRE(mockLastCommands[2] == 0x08);
	REQUIRE(mockLastCommands[3] == 0x81);
	REQUIRE(mockLastCommands[4] == 0xFB);
	REQUIRE(mockLastCommands[5] == 0x07);
	REQUIRE(mockLastCommands[6] == 0x00);
	REQUIRE(mockLastCommands[7] == 0x00);
	REQUIRE(mockLastCommands[8] == 0x26);
	REQUIRE(mockLastCommands[9] == SearchROMCommand);
	REQUIRE(mockLastCommands[10] == 0x28);
	REQUIRE(mockLastCommands[11] == 0x62);
	REQUIRE(mockLastCommands[12] == 0x24);
	REQUIRE(mockLastCommands[13] == 0xC7);
	REQUIRE(mockLastCommands[14] == 0x03);
	REQUIRE(mockLastCommands[15] == 0x00);
	REQUIRE(mockLastCommands[16] == 0x00);
	REQUIRE(mockLastCommand == 0x0F);

	rom_address_t ROM_address = One_wire::get_address(0);
	REQUIRE(ROM_address.rom[0] == 0x28);
	REQUIRE(ROM_address.rom[1] == 0x08);
	REQUIRE(ROM_address.rom[2] == 0x81);
	REQUIRE(ROM_address.rom[3] == 0xFB);
	REQUIRE(ROM_address.rom[4] == 0x07);
	REQUIRE(ROM_address.rom[5] == 0x00);
	REQUIRE(ROM_address.rom[6] == 0x00);
	REQUIRE(ROM_address.rom[7] == 0x26);

	ROM_address = One_wire::get_address(1);
	REQUIRE(ROM_address.rom[0] == 0x28);
	REQUIRE(ROM_address.rom[1] == 0x62);
	REQUIRE(ROM_address.rom[2] == 0x24);
	REQUIRE(ROM_address.rom[3] == 0xC7);
	REQUIRE(ROM_address.rom[4] == 0x03);
	REQUIRE(ROM_address.rom[5] == 0x00);
	REQUIRE(ROM_address.rom[6] == 0x00);
	REQUIRE(ROM_address.rom[7] == 0x0F);
}


TEST_CASE("ListAllAttachedDevices", "[one_wire]") {
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

	one_wire.find_and_count_devices_on_bus();

	rom_address_t ROM_address = One_wire::get_address(0);
	REQUIRE(ROM_address.rom[0] == 0x28);
	REQUIRE(ROM_address.rom[1] == 0x08);
	REQUIRE(ROM_address.rom[2] == 0x81);
	REQUIRE(ROM_address.rom[3] == 0xFB);
	REQUIRE(ROM_address.rom[4] == 0x07);
	REQUIRE(ROM_address.rom[5] == 0x00);
	REQUIRE(ROM_address.rom[6] == 0x00);
	REQUIRE(ROM_address.rom[7] == 0x26);

	ROM_address = One_wire::get_address(1);
	REQUIRE(ROM_address.rom[0] == 0x28);
	REQUIRE(ROM_address.rom[1] == 0x62);
	REQUIRE(ROM_address.rom[2] == 0x24);
	REQUIRE(ROM_address.rom[3] == 0xC7);
	REQUIRE(ROM_address.rom[4] == 0x03);
	REQUIRE(ROM_address.rom[5] == 0x00);
	REQUIRE(ROM_address.rom[6] == 0x00);
	REQUIRE(ROM_address.rom[7] == 0x0F);
}