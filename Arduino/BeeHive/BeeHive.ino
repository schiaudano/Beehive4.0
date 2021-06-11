// this library is used to write and read data from the EEPROM
#include <EEPROM.h>

// this library is used to define the Bluetotth serial
#include "SoftwareSerial.h"

// this library is used for the DHT temperature and humidity sensor
#include "DHT.h"

// this library is used for the HX711 module (scale sensor)
#include "HX711.h"

// defining the type of the DHT sensor: DHT11 or DHT22
#define DHTTYPE DHT11

// this variables are used to map the pins of the modules
static const int BT_TX_PIN = 3;
static const int BT_RX_PIN = 2;

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

// This number is obtained during the scale calibration procedure
float calibrationFactor = 26200;

long zeroFactor = 0;

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
  String ret = "T=";
  ret.concat(String(scale.get_units(hx711Samples), 2));
  ret.concat(" kg");

  bt.println(ret);
  scale.power_down();
}

void tareScale() {
  scale.power_up();
  scale.tare(hx711Samples);
  zeroFactor = scale.read_average(hx711Samples);
  // scale.set_offset(zeroFactor);
  // EEPROM.put(0, zeroFactor);
  bt.print("Scale tared. Zero factor: ");
  bt.println(zeroFactor);
  scale.power_down();
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
        bt.println("t - used to tare the scale");
        bt.println("n {name} - used to change the Bluetooth name");
        bt.println("p {pin} - change the Bluetooth pin");

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

      // this command is used to tare the scale
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
  // put your setup code here, to run once:
  dht.begin();

  pinMode(BT_RX_PIN, INPUT);
  pinMode(BT_TX_PIN, OUTPUT);

  bt.begin(115200);

  scale.begin(HX_DT_PIN, HX_SCK_PIN);
  scale.set_scale(calibrationFactor);

  // EEPROM.get(0, zeroFactor);
  // scale.set_offset(zeroFactor);
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
