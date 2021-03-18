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
    RaspiCon raspiCon;

    std::string ssid = "Lösch dein Netz!";
    std::string password = "123password";

    auto macMessage = raspiCon.listen();
    raspiCon.send(Message{KeepOpen, Ok, Answer, ssid});
    raspiCon.send(Message{KeepOpen, Ok, Answer, password});

    std::cout << "Die Mac-Adresse ist: " << macMessage.message << std::endl;

    return 0;
}
