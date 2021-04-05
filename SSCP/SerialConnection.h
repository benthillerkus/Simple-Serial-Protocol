#pragma once

#include <cstring> // For std::memcpy
#include <cstdint>

const char WAKE = 0b11001100; // Wake word to listen for

// Bitflags for control byte

enum Connection {
    KeepOpen = 0, Close = 0b10000000
};
enum Ack {
    Ok = 0, Error = 0b01000000
};
enum Type {
    Query = 0, Answer = 0b00000001
};

/** Representation of a Message.
Use `fromBytes` and `toBytes` to convert the actual data
sent over the wire. */
template<typename StringType, class MessageType>
struct Message {
    const Connection connection;
    const Ack ack;
    const Type type;
    const StringType message;

    static StringType substring(const StringType &string, size_t from) { return MessageType::substring(string, from); };

    inline bool operator==(const Message &rhs) {
        return connection == rhs.connection &&
               ack == rhs.ack &&
               type == rhs.type &&
               message == rhs.message;
    }

    inline bool operator!=(const Message &rhs) { return !(this == rhs); };

    constexpr StringType toBytes() const {
        uint8_t control = 0b00000000;
        if (connection == Close) control |= Close;
        if (ack == Error) control |= Error;
        if (type == Answer) control |= Answer;

        // Remember that C-Strings (char*) terminate on `0b00000000`!
        // Therefore we need to invert our data to prevent this
        // and pad C-Strings that we construct ourselves!
        uint16_t length = ~message.length();
        char header[4]{(char) ~control}; // 4th byte as C-String delimiter
        std::memcpy(header + 1, &length, 2); // Copy into bits 3, 4

        return StringType(header + message);
    }

    constexpr static Message fromBytes(const StringType &input) {
        uint8_t control = ~((char) input[0]);
        return Message{
                static_cast<Connection> (control & Close),
                static_cast<Ack>(control & Error),
                static_cast<Type>(control & Answer),
                substring(input, 3)
        };
    }
};

/** Represents a Serial Connection (duh).
You have to override `readSingle` and `writeBytes`.
You can manually `send` and `listen` to messages
or fill your instance with jobs and have it auto
resolve these. */
template<typename StringType, class MessageType>
class SerialConnection {
private:
    StringType wholeMessageCache;

    /** Read a single char from the Serial connection.
    You need to manage buffers and / or delays
    and prevent placeholders or duplicate values! */
    virtual char readSingle() const = 0;

    /** Write every single character from the provided String
    as is to the Serial output. */
    virtual void writeBytes(const StringType &) const = 0;

public:
    SerialConnection() = default;

    ~SerialConnection() = default;

    /** Listen for a single message on the serial connection.
    Will fucking block everything lol
    You shouldn't actually use this, but instead rely on
    the job system to do its - well - job */
    Message<StringType, MessageType> listen() {
        while (readSingle() != WAKE) {}; // Wait for wake word
        wholeMessageCache = readSingle(); // Read control byte
        // Read message length
        uint16_t length;
        auto buffer = readSingle();
        wholeMessageCache += buffer;
        std::memcpy((uint8_t *) &length, &buffer, 1);
        buffer = readSingle();
        wholeMessageCache += buffer;
        std::memcpy((uint8_t *) (&length) + 1, &buffer, 1);
        // Read the message
        length = ~length;
        for (int i = 0; i < length; i++) {
            wholeMessageCache += readSingle();
        }

        return Message<StringType, MessageType>::fromBytes(wholeMessageCache);
    };

    void send(const Message<StringType, MessageType> &message) const {
        writeBytes(StringType{WAKE});
        writeBytes(message.toBytes());
    };
};
