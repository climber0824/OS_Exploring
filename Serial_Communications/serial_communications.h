#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <cstring>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <linux/spi/spidev.h>


class UART {
private:
    int fd;

public:
    UART(const char *device = "/dev/ttyAMA0", int baud = B9600) {}

    ~UART() {}

    bool isOpen() const {}

    int write(const char *data, size_t length) {}
    int writeString(const std::string &str) {}

    int read(char *buffer, size_t length) {}

    std::string readLine(char delimiter = '\n') {}

    int available() {}
};
