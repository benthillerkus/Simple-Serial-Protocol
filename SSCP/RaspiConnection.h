#include "SerialConnection.h"
#include <CppLinuxSerial/SerialPort.hpp>

#pragma once

using namespace mn::CppLinuxSerial;

struct RaspiMessage : public Message<std::string, RaspiMessage> {
    static std::string substring(const std::string &string, size_t &from) { return string.substr(from); };
};

class RaspiCon : public SerialConnection<std::string, RaspiMessage> {
private:
    mutable SerialPort serialPort;

    char readSingle() const override {
        static std::string buffer;
        static std::string readInto;

        if (buffer.length() == 0) {
            serialPort.Read(readInto);
            buffer = readInto.substr(1);
            readInto = readInto[0];
        } else {
            readInto = buffer[0];
            buffer.erase(0, 1);
        }
        return readInto[0];
    }

    void writeBytes(const std::string &input) const override {
        serialPort.Write(input);
    }

public:
    explicit RaspiCon() :
            serialPort("/dev/ttyS0", BaudRate::B_115200) {
        serialPort.SetTimeout(-1);
        serialPort.Open();
    }

    ~RaspiCon() {
        serialPort.Close();
    }
};
