#pragma once

#include <string>
#include <cstring>
#include <vector>

typedef std::string stringType;
struct Message;
struct Job;
const auto WAKE = 0b11001100;

class SerialConnection {
private:
    std::vector<Job> jobs;
    stringType wholeMessageCache;
public:
    SerialConnection() = default;
    ~SerialConnection() = default;

    virtual Message listen() = 0;
    virtual void send(Message) = 0;

    void addJob(const Job& job) {
        jobs.push_back(job);
    }
};

enum Connection {
    KeepOpen = 0, Close = 0b10000000
};

enum Ack {
    Ok = 0, Error = 0b01000000
};

enum Type {
    Query = 0, Answer = 0b00000001
};

typedef Ack(*JobCallback)(const Message&);
struct Job {
    const stringType message;
    const JobCallback callback;
};

struct Message {
    const Connection connection;
    const Ack ack;
    const Type type;
    const stringType message;

    stringType toBytes() {
        uint8_t control = 0b00000000;
        if (connection == Close) control |= Close;
        if (ack == Error) control |= Error;
        if (type == Answer) control |= Answer;

        uint16_t length = ~message.length();
        char header[5]{(char) WAKE, (char) ~control}; // 5th byte as C-String delimiter
        std::memcpy(header + 2, &length, 2);

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

