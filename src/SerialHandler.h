#ifndef SERIALHANDLER_H
#define SERIALHANDLER_H

#include "../SerialPort/SerialPort.hpp"


class SerialHandler {
    SerialPort *serial;
    int message_i;
    bool start_tracking;
    char message_buffer[16];

public:
    char message[16];
    bool has_message;

    SerialHandler();
    ~SerialHandler();
    void write(const std::string& msg) const;
    void read();
};


#endif
