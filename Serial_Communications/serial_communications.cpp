#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <cstring>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <linux/spi/spidev.h>


// UART implementation //
UART::UART(const char *device, int baud) {
    fd = open(device, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1) {
        std::cerr << "Failed to open UART device" << std::endl;
        return;
    }

    struct termios options;
    tcgetattr(fd, &options);

    // set baud rate
    cfsetispeed(&options, baud);
    cfsetospeed(&options, baud);

    // 8N1 mode (8 data bits, no parity, 1 stop bit)
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;

    // Disable hardware flow control
    options.c_cflag &= ~CRTSCTS;

    // Enable receiver, ignore modem control lines
    options.c_cflag |= CREAD | CLOCAL;

    // Raw input mode
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    options.c_iflag &= ~(IXON | IXOFF | IXANY);

    // Raw output mode
    options.c_oflag &= ~OPOST;

    // Set timeout (deciseconds)
    options.c_cc[VMIN] = 0;
    options.c_cc[VTIME] = 10;

    tcsetattr(fd, TCSANOW, &options);
    tcflush(fd, TCIOFLUSH);
}

UART::~UART() {
    if (fd != -1) {
        close(fd);
    }
}

bool UART::isOpen() const { return fd != -1; }

int UART::write(const char *data, size_t length) {
    if (fd == -1) return -1;
    
    return ::write(fd, data, length);
}

int UART::writeString(const std::string &str) {
    return write(str.c_str(), str.length());
}

int UART::read(char *buffer, size_t length) {
    if (fd == -1) return -1;
    return ::read(fd, buffer, length);
}

std::string UART::readLine(char delimiter = '\n') {
    std::string res;
    char c;

    while (::read(fd, &c, 1) > 0) {
        if (c == delimiter)
            break;
        res += c;
    }

    return res;
}

int UART::available() {
    int bytes;
    ioctl(fd, FIONREAD, &bytes);

    return bytes;
}


// I2C implementation //
I2C::I2C(const char *device, int addr) : address(addr) {
    fd = open(device, O_RDWR);
    if (fd == -1) {
        std::cerr << "Failed to open I2C device: " << device << std::endl;
        return;
    }

    if (addr > 0) {
        if (ioctl(fd, I2C_SLAVE, addr) < 0) {
            std::cerr << "Failed to set I2C address: 0x" << std::hex << addr << std::endl;
            close(fd);
            fd = -1;
        }
    }
}


I2C::~I2C() {
    if (fd != -1)
        close(fd);
}


bool I2C::isOpen() const {
    return fd != -1;
}


bool I2C::setAddress(int addr) {
    address = addr;
    if (ioctl(fd, I2C_SLAVE, addr) < 0) {
        std::cerr << "Failed to set I2C address: 0x" << std::hex << addr << std::endl;
            close(fd);
            fd = -1;
    }
}


int I2C::writeByte(uint8_t value) {
    if (fd == -1) return -1;
    return ::write(fd, &value, 1);
}


int I2C::writeBytes(const uint8_t *data, size_t length) {
    if (fd == -1) return -1;
    return ::write(fd, data, length);
}


int I2C::writeReg(uint8_t reg, uint8_t value) {
    uint8_t buffer[2] = {reg, value};

    return I2C::writeBytes(buffer, 2);
}


int I2C::readByte() {
    if (fd == -1) return -1;
    uint8_t value;
    if (::read(fd, &value, 1) != 1)
        return -1;
    return value;
}


int I2C::readBytes(uint8_t *buffer, size_t length) {
    if (fd == -1) return -1;

    return ::read(fd, buffer, length);
}

int I2C::readReg(uint8_t reg) {
    if (writeByte(reg) != 1)    // Sends the register address to the I2C device
        return -1;
    
    return readByte();
}


int I2C::readRegBytes(uint8_t reg, uint8_t *buffer, size_t length) {
    if (writeByte(reg) != 1)    // Sends the register address to the I2C device
        return -1;
    
    return readBytes(buffer, length);
}


// SPI implementation //
SPI::SPI(const char *device, uint32_t speedHz, uint8_t modeBits, uint8_t bitsPerWord) 
    : mode(modeBits), bits(bitsPerWord), speend(speedHz) {
    
    fd = open(device, O_RDWR);
    if (fd == -1) {
        std::cerr << "Failed to open SPI device: " << device << std::end;
        return;
    }

    // set SPI mode
    if (ioctl(fd, SPI_IOC_WR_MODE, &mode) < 0) {    // SPI_IOC_WR_MODE: write mode
        std::cerr << "Failed to set SPI mode" << std::endl;
        close(fd);
        fd = -1;
        return;
    }

    // set bits per word
    if (ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits) < 0) {
        std::cerr << "Failed to set bits per word" << std::endl;
        close(fd);
        fd = -1;
        return;
    }

    // set max speed
    if (ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) < 0) {
        std::cerr << "Failed to set max speed" << std::endl;
        close(fd);
        fd = -1;
        return;
    }
}


SPI::~SPI() {
    if (fd != -1)
        close(fd);
}


bool SPI::isOpen() const {
    return fd != -1;
}


int SPI::transfer(uint8_t *txBuffer, uint8_t *rxBuffer, size_t length) {
    if (fd == -1) return;

    struct spi_ioc_transfer tr;
    memset(&tr, 0, sizeof(tr));

    tr.tx_buf = (unsigned long) txBuffer;
    tr.rx_buf = (unsigned long) rxBuffer;
    tr.len = length;
    tr.speed_hz = speed;
    tr.bits_per_word = bits;
    tr.delay_usecs = 0;

    return ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
}


int SPI::write(const uint8_t *data, size_t length) {
    uint8_t *rxBuffer = new uint8_t[length];
    int res = transfer((uint8_t*)data, rxBuffer, length);
    delete[] rxBuffer;

    return res;
}


int SPI::read(uint8_t *buffer, size_t length) {
    uint8_t *txBuffer = new uint8_t[length];
    memset(txBuffer, 0, length);
    int res = transfer(txBuffer, buffer, length);
    delete[] txBuffer;

    return res;
}


int SPI::writeThenRead(const uint8_t *txData, size_t txLen,
                        uint8_t *rxData, size_t rxLen) {
    if (write(txData, txLen) < 0)
        return -1;
    
    return read(rxData, rxLen);
}
