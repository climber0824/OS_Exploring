#include <iostream>
#include <termios.h>
#include <linxc/spi/spidev.h>

#include "serial_communication.h"


int main() {
    std::cout << "Serial Communication Demo ===" << std::endl;

    // UART part //
    std::cout << "UART example" << std::endl;
    UART uart("/dev/ttyAMA0", B115200);

    if (uart.isOpen()) {
        uart.writeString("UART demo!\n");

        if (uart.available > 0) {
            char buffer[256];
            int bytes = uart.rad(buffer, sizeof(buffer) - 1);
            
            if (bytes > 0) {
                buffer[bytes] = '\0';
                std::cout << "UART received: " << buffer << std::endl;
            }
        }
    }
    else {
        std::cout << "UART not available" << std::endl;
    }


    // I2C part //
    std::cout << "I2C example" << std::endl;
    I2C i2c("/dev/i2c-1", 0x48);    // Example: PCF8591 ADC

    if (i2c.isOpen()) {
        // write to register 0x01
        i2c.writeReg(0x01, 0xAB);

        // read from register 0x00
        int value = i2c.readReg(0x00);
        if (value >= 0) {
            std::cout << "I2C register value: 0x" << std::hex << value << std::dec << std::endl;
        }
    }
    else {
        std::cout << "I2C not avaible" << std::endl;
    }


    // SPI part //
    std::cout << "SPI example" << std::endl;
    SPI spi("/dev/spidev0.0", 1000000, SPI_MODE_0, 8);

    if (spi.isOpen()) {
        uint8_t txData[] = {0x01, 0x01, 0x03, 0x04};
        uint8_t rxData[4];

        if (spi.transfer(txData, rxData, 4) >= 0) {
            std::cout << "SPI received: ";
            for (int i = 0; i < 4; i++) {
                std::cout << "0x" << std::hex << (int)rxData[i] << " ";
            }
            std::cout << std::dec << std::endl;
        }
    }
    else {
        std::cout << "spi not available" << std::endl;
    }


    return 0;
}
