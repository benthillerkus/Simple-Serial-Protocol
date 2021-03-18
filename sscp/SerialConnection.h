#pragma once

#include <string>
#include <cstring> // For std::memcpy
#include <vector>

typedef std::string stringType;
const auto WAKE = 0b11001100; // Wake word to listen for

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
struct Message {
    const Connection connection;
    const Ack ack;
    const Type type;
    const stringType message;

    stringType toBytes() const {
        uint8_t control = 0b00000000;
        if (connection == Close) control |= Close;
        if (ack == Error) control |= Error;
        if (type == Answer) control |= Answer;

        // Remember that C-Strings (char*) terminate on `0b00000000`!
        // Therefore we need to invert our data to prevent this
        // and pad C-Strings that we construct ourselves!
        uint16_t length = ~message.length();
        char header[5]{(char) WAKE, (char) ~control}; // 5th byte as C-String delimiter
        std::memcpy(header + 2, &length, 2); // Copy into bits 3, 4

        return stringType(header + message);
    }

    static Message fromBytes(const stringType &input) {
        uint8_t control = ~input[0];
        return Message{
                static_cast<Connection> (control & Close),
                static_cast<Ack>(control & Error),
                static_cast<Type>(control & Answer),
                input.substr(3)
        };
    }
};

typedef Ack(*JobCallback)(const Message &);

struct Job {
    const stringType message;
    const JobCallback callback;
};

class SerialConnection {
private:
    std::vector <Job> jobs;
    stringType wholeMessageCache;

    /** Read a single char from the Serial connection.
    You need to manage buffers and / or delays
    and prevent placeholders or duplicate values! */
    virtual char readSingle() = 0;

    /** Write every single character from the provided String
    as is to the Serial output. */
    virtual void writeBytes(const stringType &) = 0;

public:
    SerialConnection() = default;

    ~SerialConnection() = default;

    /** Listen for a single message on the serial connection.
    Will fucking block everything lol
    You shouldn't actually use this, but instead rely on
    the job system to do its - well - job */
    Message listen() {
        while (readSingle() != WAKE) {}; // Wait for wake word
        wholeMessageCache = readSingle(); // Read control byte
        // Read message length
        uint16_t length;
        auto buffer = readSingle();
        wholeMessageCache += buffer;
        std::memcpy((uint8_t * ) & length, &buffer, 1);
        buffer = readSingle();
        wholeMessageCache += buffer;
        std::memcpy((uint8_t * )(&length) + 1, &buffer, 1);
        // Read the message
        length = ~length;
        for (int i = 0; i < length; i++) {
            wholeMessageCache += readSingle();
        }

        return Message::fromBytes(wholeMessageCache);
    };

    void send(const Message &message) {
        writeBytes(message.toBytes());
    };

    /** The connection will not close until the provided job
    has been completed. Use the callback field on the Job struct
    to deal with the response. */
    void addJob(const Job &job) {
        jobs.push_back(job);
    }
};
