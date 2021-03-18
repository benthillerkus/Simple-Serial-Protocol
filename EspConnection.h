#include <SerialConnection.h>

class EspCon : public SerialConnection {
private:
    char readSingle() const override {
        // Serial.read() is a non-blocking call.
        // We turn it into a polling, blocking one 😬
        while (!Serial.available()) {
            delay(5);
        }
        return Serial.read();
    }

    void writeBytes(const stringType &input) const override {
        Serial.write(input.c_str());
    }

public:
    EspCon() {
        Serial.begin(115200);
    }

    ~EspCon() {
        Serial.end();
    }
};
