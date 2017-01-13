# micro:bit OneWire Library

Based upon Erik Olieman's mbed DS1820 lib

https://developer.mbed.org/components/DS1820/

Modified timings so that it works when used on the Micro:Bit.

Added off device test code with mocks and its own cmake configuration.

Modified API's for two stage workflow:
1. Listing of devices with addresses to be stored elsewhere
2. Returning temperatures for each device by address
