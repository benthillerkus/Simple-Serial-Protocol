#include <SerialConnection.h>
#include <CppLinuxSerial/SerialPort.hpp>

using namespace mn::CppLinuxSerial;

class RaspiCon : public SerialConnection {
private:
    SerialPort serialPort;
    std::string buffer;
    std::string readInto;
    std::string wholeMessageCache;

    void readSingle() {
        if (buffer.length() == 0) {
            serialPort.Read(readInto);
            buffer = readInto.substr(1);
            readInto = readInto[0];
        } else {
            readInto = buffer[0];
            buffer.erase(0, 1);
        }
    }

public:
    RaspiCon() :
            serialPort("/dev/ttyS0", BaudRate::B_115200) {
        serialPort.SetTimeout(-1);
        serialPort.Open();
    }

    ~RaspiCon() {
        serialPort.Close();
    }

    Message listen() override {
        wholeMessageCache = "";
        while(readInto[0] != WAKE) {
            readSingle();
        }
        // Read Control
        readSingle();
        wholeMessageCache += readInto;
        // Read First Byte of Message Length
        readSingle();
        wholeMessageCache += readInto;
        uint16_t length = 0;
        std::memcpy((void *) &length, &readInto[0], 1);
        // Read Second Byte of Message Length
        readSingle();
        wholeMessageCache += readInto;
        std::memcpy((uint8_t *) (&length) + 1, &readInto[0], 1);
        // Read the Message
        length = ~length;
        for (int i = 0; i < length; i++) {
            readSingle();
            wholeMessageCache += readInto;
        }

        return Message::fromBytes(wholeMessageCache);
    }

    void send(Message message) override {
        serialPort.Write(message.toBytes());
    }
};

int main() {
    RaspiCon con;

    con.addJob(Job{"MAC", [](const Message& message){
        std::cout << "MAC ist: " << message.message << std::endl;
        return Ok;
    }});

    auto message = con.listen();
    std::cout << message.message << std::endl;

    con.send(Message{KeepOpen, Ok, Query, "Wie geht's so Ü?"});
    auto message2 = con.listen();
    std::cout << message2.message << std::endl;

    return 0;
}