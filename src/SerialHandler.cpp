#include "SerialHandler.h"
#include <format>


SerialHandler::SerialHandler() : serial(new SerialPort("COM3")), message_i(0), start_tracking(false),
message_buffer{}, message{}, has_message(false) {
    if (!serial->isConnected()) {
        throw std::runtime_error("Error opening serial port.");
    }
}

SerialHandler::~SerialHandler() {
    serial->closeSerial();
}

void SerialHandler::write(const std::string& msg) const {
    if (!serial->writeSerialPort(("<" + msg + ">").c_str(), 16)) {
        throw std::runtime_error("Error sending serial.");
    }
}

void SerialHandler::read() {
    if (has_message) {
        return;
    }

    int num_bytes = serial->readSerialPort(message_buffer, 1);

    for (int i = 0; i < num_bytes; i++) {
        const char c = message_buffer[i];
        if (start_tracking) {
            if (c != '>' && message_i < 15) {
                message[message_i++] = c;
            } else {
                start_tracking = false;
                message[message_i] = '\0';
                has_message = true;
                message_i = 0;
            }
        } else if (c == '<') {
            start_tracking = true;
        }
    }
}
