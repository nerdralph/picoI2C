/* Small i2c master for AVR devices
 * Ralph Doncaster (c) 2020
 * free software - MIT license
 * v 0.5 20200102
 */

#pragma once

#include <stdint.h>
#include <avr/io.h>

#define I2C_PORT PORTB 
#define I2C_DDR DDRB
#define I2C_PIN PINB

const int SUCCESS = 0;
const int ADDR_NAK = 2;
const int BUS_INUSE = 5;

class picoI2C {
public:
    static uint8_t startWrite(uint8_t addr)
    {
        uint8_t err = start();
        if (err) return err;
        return write(addr<<1);          // address goes in upper 7 bits
    }
    
    // start must be called first
    static uint8_t write(uint8_t data)
    {
        rw(data);
        scl_hi();
    
        int err = SUCCESS;
        // NACK = SDA high
        if (I2C_PIN & (1<<SDA)) err = ADDR_NAK;
        I2C_DDR |= (1<<SCL);        // SCL low
    
        return err;
    }

    static char readAck() { return read(1); }

    static char readNack() { return read(0); }

    static uint8_t startRead(uint8_t addr)
    {
        start();
        return write(addr<<1 | 1);      // address goes in upper 7 bits
    }
    
    static uint8_t restartRead(uint8_t addr)
    {
        scl_hi();
        return startRead(addr);
    }
    
    static void stop()
    {
        I2C_DDR |= (1<<SDA);        // SDA low
        I2C_DDR &= ~(1<<SCL);       // release SCL
        asm ("lpm" ::: "r0");           // 3 cycle delay
        I2C_DDR &= ~(1<<SDA);       // release SDA
    }
    
    // ack !=0 to send ack
    static uint8_t read(uint8_t ack)
    {
        uint8_t data = rw(0xFF);
    
        if (ack)
            I2C_DDR |= (1<<SDA);    // SDA low = ACK
        scl_hi();
        asm ("nop");                    // delay
        I2C_DDR |= (1<<SCL);        // SCL low
    
        return data; 
    }

private:
    static uint8_t start()
    {
        // SDA should be high if bus is free
        if ((I2C_PIN & (1<<SDA)) == 0)
            return BUS_INUSE;
        // set SDA to output mode for low 
        I2C_DDR |= (1<<SDA);
        asm ("lpm" ::: "r0");       // 3 cycle delay
        // set SCL to output mode for low 
        I2C_DDR |= (1<<SCL);
        return 0;
    }

    __attribute__((always_inline))\
    static inline void scl_hi()
    {
        // set SCL to input mode for high
        I2C_DDR &= ~(1<<SCL);
        // check for clock stretching
        while ((I2C_PIN & (1<<SCL)) == 0);
    }
    
    // combined i2c read/write routine
    // pass 0xFF when reading
    static uint8_t rw(uint8_t data)
    {
        uint8_t bits = 8;
        do{
            I2C_DDR &= ~(1<<SDA);   // SDA high
            if (!(data & 0x80)) I2C_DDR |= (1<<SDA);
            scl_hi();
            uint8_t i2c_pin = I2C_PIN;
            I2C_DDR |= (1<<SCL);    // SCL low
            if (i2c_pin & (1 << SDA))
                data |= 0x01;
            data <<= 1;
        } while (--bits);
    
        I2C_DDR &= ~(1<<SDA);       // release SDA
        return data;
    }

    // PORT pin numbers for SDA & SCL (0-7)
    static const uint8_t SCL;
    static const uint8_t SDA;

};

