#ifndef SERIAL_COMM_H
#define SERIAL_COMM_H

#include <string>
#include <cstdint>

class UART {
private:
    int fd;

public:
    UART(const char *device = "/dev/ttyAMA0", int baud = B9600);

    ~UART();

    bool isOpen() const;
    int write(const char *data, size_t length);
    int writeString(const std::string &str);
    int read(char *buffer, size_t length);
    std::string readLine(char delimiter = '\n');
    int available();
};


class I2C {
private:
    int fd;
    int address;

public:
    I2C(const char *device = "/dev/i2c-1", int addr = 0x00);    
    ~I2C();

    bool isOpen() const;
    bool setAddress(int addr);
    int writeByte(uint8_t value);
    int writeBytes(const uint8_t *data, size_t length);
    int writeReg(uint8_t reg, uint8_t value);
    int readByte();
    int readBytes(uint8_t *buffer, size_t length);
    int readReg(uint8_t reg);
    int readRegBytes(uint8_t reg, uint8_t *buffer, size_t length);
};


class SPI {
private:
    int fd;
    uint8_t mode;
    uint8_t bits;
    uint32_t speed;

public:
    SPI(const char *device = "/dev/spidev0.0",
        uint32_t speedHz = 1e6,
        uint8_t modeBits = 0,
        uint8_t bitsPerWord = 8);
    
    ~SPI();

    bool isOpen() const;
    int transfer(uint8_t *txBuffer, uint8_t *rxBuffer, size_t length);
    int write(const uint8_t *data, size_t length);
    int read(uint8_t *buffer, size_t length);
    int writeThenRead(const uint8_t *txData, size_t txLen,
                      uint8_t *rxData, size_t rxLen);
};

#endif
