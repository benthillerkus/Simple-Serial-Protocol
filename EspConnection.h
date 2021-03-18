#include <SerialConnection.h>

class EspCon : SerialConnection {
private:
    std::string wholeMessageCache;
public:
    EspCon() {
        Serial.begin(115200);
    }

    ~EspCon() {
        Serial.end();
    }

    Message listen() override {
        while(Serial.read() != WAKE) {delay(5);};
        // Read Control
        wholeMessageCache = Serial.read();
        // Read Length First Byte
        uint16_t length = 0;
        char current = Serial.read();
        wholeMessageCache += current;
        std::memcpy((void*)&length, &current, 1);
        // Read Length Second Byte
        current = Serial.read();
        wholeMessageCache += current;
        std::memcpy((uint8_t*)(&length)+1, &current, 1);
        // Read The Message
        length = ~length;
        for(int i = 0; i < length; i++) {
            wholeMessageCache += Serial.read();
        }

        return Message::fromBytes(wholeMessageCache);
    }

    void send(Message message) override {
        Serial.write(message.toBytes().c_str());
    }
};