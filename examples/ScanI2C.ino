/* Small i2c scanner example for picoI2C
 * https://github.com/nerdralph/picoI2C
 * Ralph Doncaster (c) 2019
 * free software - MIT license
 */

// converts 4-bit nibble to ascii hex
uint8_t nibbletohex(uint8_t value)
{
    if ( value > 9 ) value += 'A' - ':';
        return value + '0';
}

void printHex()

// PORT pin numbers for SDA & SCL (0-7)
const uint8_t PicoI2C::SCL = 4;
const uint8_t PicoI2C::SDA = 3;

void loop()
{
    // scan bus
    uint8_t addr = 8;
    const uint8_t addr_end = 119;
    do {
        if (PicoI2C::startWrite(addr) == 0) {
        //if (i2c_start_read(addr) == 0) {
        }
        PicoI2C::stop();
    } while (++addr <= addr_end);
}
