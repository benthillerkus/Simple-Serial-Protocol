#include <RaspiConnection.h>

int main() {
    RaspiCon raspiCon;

    std::string ssid = "Lösch dein Netz!";
    std::string password = "123password";

    auto const messageSsid = RaspiMessage{KeepOpen, Ok, Answer, ssid};
    auto const messagePassword = RaspiMessage{KeepOpen, Ok, Answer, password};

    raspiCon.send(messageSsid);
    raspiCon.send(messagePassword);
    auto macMessage = raspiCon.listen();
    auto shouldEqualMessageSsid = raspiCon.listen();

    std::cout << "Die Mac-Adresse ist: " << macMessage.message << std::endl;
    std::cout << "Und außerdem ist die SSID "
              << (shouldEqualMessageSsid == messageSsid ? "korrekt" : "falsch")
              << " übertragen worden!" << std::endl;

    return 0;
}
