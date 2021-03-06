#include "DHT.h"
#include "SoftwareSerial.h"

#define DHTTYPE DHT11

static const int DHT_PIN = 4;

static const int BT_TX_PIN = 3;
static const int BT_RX_PIN = 2;

SoftwareSerial bt = SoftwareSerial(BT_RX_PIN, BT_TX_PIN);

String btName = "";
String btPin = "";

DHT dht(DHT_PIN, DHTTYPE);

String leggiAmbiente() {
  String ret = "T=";
  ret.concat(dht.readTemperature());
  ret.concat(char(159));
  ret.concat("C, H=");
  ret.concat(dht.readHumidity());
  ret.concat("%");

  return ret;
}

void setup() {
  // put your setup code here, to run once:
  dht.begin();

  pinMode(BT_RX_PIN, INPUT);
  pinMode(BT_TX_PIN, OUTPUT);

  bt.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  String txt = "";
  if (bt.available()) {
    txt = bt.readString();
    char cmd = txt.charAt(0);
    switch (cmd) {
      case 'a':
        bt.println(leggiAmbiente());
        break;

      case 'n':
        btName = txt.substring(2);
        if(btName.length() < 2) {
          bt.println("Error. Nome non valido");
          btName = "";
        }
        
        break;

      case 'p':
         btPin = txt.substring(2);
        if(btPin.length() < 4) {
          bt.println("Error. Pin non valido");
          btPin = "";
        }
        break;

      default:
        bt.print("Error. Comando non riconosciuto:");
        bt.println(txt);
        break;
    }
  }

  if(!btName.equals("")) {
    bt.print("AT+BDNAME=");
    bt.print(btName);
    bt.write("\r\n");
    bt.print("Sto cambiando il nome in:");
    bt.println(btName);
    delay(5000);
  }

  if(!btPin.equals("")) {
    bt.print("AT+BDPIN=");
    bt.print(btPin);
    bt.write("\r\n");
    bt.print("Sto cambiando il pin in:");
    bt.println(btPin);
    delay(5000);
  }
}
