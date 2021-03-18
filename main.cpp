#include <SerialConnection.h>
#include <CppLinuxSerial/SerialPort.hpp>

using namespace mn::CppLinuxSerial;

class RaspiCon : public SerialConnection {
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

    void writeBytes(const stringType &input) const override {
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

int main() {
    RaspiCon con;

    con.addJob(Job{"MAC", [](const Message &message) {
        std::cout << "MAC ist: " << message.message << std::endl;
        return Ok;
    }});

    auto message = con.listen();
    std::cout << message.message << std::endl;

    con.send(Message{KeepOpen, Ok, Query, "Wie geht's so Ü?"});
    auto message2 = con.listen();
    std::cout << message2.message << std::endl;

    std::move(con).autoResolve([](const Message &message) {
        if (message.type == Query) {
            if (message.message == "PASSWORD") {
                return Response{Ok, "123 Passwort :)"};
            } else if (message.message == "IP") {
                return Response{Ok, "127.0.0.0"};
            }
        }
        return Response{Error, ""};
    });
    return 0;
}
