#include "serial_communication.h"

// UART //
class UART {
private:
    int fd;

public:
    UART(const char *device = "/dev/ttyAMA0", int baud = B9600) {
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

    ~UART() {
        if (fd != -1) {
            close(fd);
        }
    }

    bool isOpen() const { return fd != -1; }

    int write(const char *data, size_t length) {
        if (fd == -1) return -1;
        
        return ::write(fd, data, length);
    }

    int writeString(const std::string &str) {
        return write(str.c_str(), str.length());
    }

    int read(char *buffer, size_t length) {
        if (fd == -1) return -1;
        return ::read(fd, buffer, length);
    }

    std::string readLine(char delimiter = '\n') {
        std::string res;
        char c;

        while (::read(fd, &c, 1) > 0) {
            if (c == delimiter)
                break;
            res += c;
        }

        return res;
    }

    int available() {
        int bytes;
        ioctl(fd, FIONREAD, &bytes);

        return bytes;
    }
};
