#include <SerialConnection.h>

void bell(int pin = 4, int length = 25) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, HIGH);
    delay(length);
    digitalWrite(pin, LOW);
}

struct EspMessage : public Message<String, EspMessage> {
    static String substring(const String &string, size_t from) {
        return string.substring(from);
    }

    EspMessage(Connection connection, Ack ack, Type type, String message) : Message<String, EspMessage>{
            connection, ack, type, message
    } {}
};

class EspCon : public SerialConnection<String, EspMessage> {
private:
    char readSingle() const override {
        // Serial.read() is a non-blocking call.
        // We turn it into a polling, blocking one 😬
        while (!Serial.available()) {
            delay(5);
        }
        return Serial.read();
    }

    void writeBytes(const String &input) const override {
        bell(2);
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
