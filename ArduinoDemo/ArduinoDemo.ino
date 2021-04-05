#include <EspConnection.h>
#include <WiFi.h>

void setup() {
  EspCon espCon;

  auto message1 = espCon.listen();
  bell();
  delay(400);
  auto message2 = espCon.listen();
  bell();
  delay(400);

  espCon.send(EspMessage{Close, Ok, Answer, WiFi.macAddress()});
  bell();
  delay(400);
  espCon.send(message1);
  bell();
  delay(400);
}

void loop() {
  // put your main code here, to run repeatedly:

}
