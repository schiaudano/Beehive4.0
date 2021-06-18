/*
Questo sketch serve per rilevare i dati di peso, temperatura e umidità dell'alveare
tramite una connessione bluetooth

Elenco comandi

i - restituisce l'elenco dei comandi con la descrizione
a - restituisce temperatura, umidità e peso
w - restituisce il peso in kg
t - azzera la bilancia
e - restituisce temperatura e unidità
n - cambia il nome del blouetooth, minimo due caratteri: n {nome bluetooth}
p - cambia la password del blouetooth, minimo quattro caratteri: p {password bluetooth}


*/

// this library is used to define the Bluetotth serial
#include "SoftwareSerial.h"

// this library is used for the DHT temperature and humidity sensor
#include "DHT.h"

// this library is used for the HX711 module (scale sensor)
#include "HX711.h"

// defining the type of the DHT sensor: DHT11 or DHT22
#define DHTTYPE DHT11

// this variables are used to map the pins of the modules
static const int BT_RX_PIN = 3;
static const int BT_TX_PIN = 2;

static const int DHT_PIN = 4;

static const int HX_DT_PIN = 5;
static const int HX_SCK_PIN = 6;

// this valiable is used to set the number of samples used by the HX711 module
static const int hx711Samples = 1;

// Creating the Bluetooth serial instance
SoftwareSerial bt = SoftwareSerial(BT_RX_PIN, BT_TX_PIN);

// Creating the instance for the temperature and humidity module
DHT dht(DHT_PIN, DHTTYPE);

// Creating the instance for the scale sensor
HX711 scale;

// Questo valore è quello ottenuto con la taratura
float calibrationFactor = -29220;

// The name and the pin of the Bluetooth module
String btName = "";
String btPin = "";

// This method returns the themperature and the humidity
void getEnvironment() {
  String ret = "T=";
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  if (isnan(temp) || isnan(hum)) {
    bt.println("Error: temperature & humidity sensor faiulure");
  }

  ret.concat(temp);
  ret.concat(char(159));
  ret.concat("C, H=");
  ret.concat(hum);
  ret.concat("%");

  bt.println(ret);
}

void getWeight() {
  scale.power_up();
  String ret = "W=";
  ret.concat(String(scale.get_units(hx711Samples), 2));
  ret.concat(" kg");

  scale.power_down();
  bt.println(ret);
}

void tareScale() {
scale.power_up();
  scale.tare(hx711Samples);
  scale.power_down();
  bt.println("Scale tared");
}

// checking for incoming commands vis Bluetooth
void checkCommands() {
  String txt = "";
  if (bt.available()) {
    txt = bt.readString();
    char cmd = txt.charAt(0);
    switch (cmd) {
      // this command returns info
      case 'i':
        bt.println("Beehive 4.0 command list:");
        bt.println("a -returns temperature, humidity and weight values");
        bt.println("e - returns temperature, humidity values");
        bt.println("w - returns the weight value");
        bt.println("n {name} - changes the Bluetooth name");
        bt.println("p {pin} - changes the Bluetooth pin");

        break;
        
      // this command returns temperature, humidity and weight values
      case 'a':
        getEnvironment();
        getWeight();
        
        break;
        
      // this command returns temperature, humidity values
      case 'e':
        getEnvironment();
        
        break;

      // this command returns the weight value
      case 'w':
        getWeight();
        
        break;

        // this command resets the scale
      case 't':
        tareScale();
        
        break;

      // this command is used to change the Bluetooth name
      case 'n':
        btName = txt.substring(2);
        if (btName.length() < 2) {
          bt.println("Error: Invlid Bluetooth name");
          btName = "";
        }

        break;

      // this command is used to change the Bluetooth pin
      case 'p':
        btPin = txt.substring(2);
        if (btPin.length() < 4) {
          bt.println("Error: Invalid Bluetooth Pin");
          btPin = "";
        }

        break;

      default:
        bt.print("Error: Unknown command ");
        bt.println(txt);

        break;
    }
  }
}

// this method is used to change the Bluetooth name sending AT command to the Bluetooth module
void changeBTName() {
  bt.print("AT+BDNAME=");
  bt.print(btName);
  bt.write("\r\n");
  bt.print("Changing Bluetooth name in: ");
  bt.println(btName);
  delay(5000);
}

// this method is used to change the Bluetooth pin sending AT command to the Bluetooth module
void changeBTPin() {
  bt.print("AT+BDPIN=");
  bt.print(btPin);
  bt.write("\r\n");
  bt.print("Changing Bluetooth pin in: ");
  bt.println(btPin);
  delay(5000);
}

void setup() {
  dht.begin();

  pinMode(BT_RX_PIN, INPUT);
  pinMode(BT_TX_PIN, OUTPUT);

  bt.begin(115200);

  scale.begin(HX_DT_PIN, HX_SCK_PIN);
  scale.set_scale(calibrationFactor);

  scale.power_down();
}

void loop() {
  // put your main code here, to run repeatedly:
  checkCommands();

  if (!btName.equals("")) {
    changeBTName();
  }

  if (!btPin.equals("")) {
    changeBTPin();
  }
}
