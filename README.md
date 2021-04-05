# Simple Serial Protocol
## Beschreibung
`SerialConnection.h` ist eine single Header file library, die Teile eines einfachen Protokolls zur Übertragung von Textnachrichten über die serielle Schnittstelle ermöglicht. [Eine detailliertere Beschreibung des Protokolls findet sich in meinem Notizbuch](https://www.notion.so/bentbent/Simples-Daten-bertragungsprotokoll-20aaba148639483981f134d8973623c3)

Im Master-Branch implementiert ist das Senden und Empfangen von einzelnen Nachrichten.

## Inhalt
- Die Library in `SerialConnection.h`
- Eine Implementierung der Library für Arduino in `EspConnection.h`
- Eine Implementierung der Library für Linux in Verwendung der Library [CppLinuxSerial](https://github.com/gbmhunter/CppLinuxSerial) in `RaspiConnection.h`
- Eine Demo in der Daten hin und zurückgeschickt werden um einen Access Point mit Webserver zu starten in `ArduinoDemo.ino` und `main.cpp`

## Verwendung
1. Du musst die Library in dein Projekt einbinden (lol viel glück wenn du cmake benutzt)
2. Erbe die Klasse `SerialConnection` und implementiere die Methoden `readSingle` und `writeBytes`
3. Erbe den Struct `Message` und implementiere `substring`
4. Du kannst nun eine neue Instanz deiner Klasse erstellen und mit `listen` und `send` Nachrichten schicken

### Installation der Library in Arduino
Kopiere den Ordner SSCP in deinen Libraries-Ordner, unter Windows ist das `C:\Users\BENUTZERNAME\Documents\Arduino\libraries`.
Mehr Infos gibt es dazu in der [offiziellen Dokumentation von Arduino](https://www.arduino.cc/en/hacking/libraries)

## Kaviar
Alles ist komplett synchron und Fehler, die auftreten könnten, werden größtenteils ignoriert.
