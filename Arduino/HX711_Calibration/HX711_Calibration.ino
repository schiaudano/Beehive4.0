/*

  Arduino pin
  5 -> HX711 DT
  6 -> HX711 SCK
  5V -> HX711 VCC
  GND -> HX711 GND

*/

#include "HX711.h"
#include "SoftwareSerial.h"

static const int HX_DT_PIN = 5;
static const int HX_SCK_PIN = 6;

HX711 scale;

static const int BT_TX_PIN = 3;
static const int BT_RX_PIN = 2;

SoftwareSerial bt = SoftwareSerial(BT_RX_PIN, BT_TX_PIN);

float calibrationFactor = 25900; //-29220 worked for my 200 kg max scale setup
long zeroFactor;
int samples = 10;

void setup() {
  bt.begin(115200);

  Serial.begin(9600);
  Serial.println("HX711 calibration");
  Serial.println("Remove all weight from scale");
  Serial.println("After readings begin, place known weight on scale");
  Serial.println("Press a to increase calibration factor");
  Serial.println("Press z to decrease calibration factor");
  Serial.println("Press t to tare the scale");
  Serial.print("Samples: ");
  Serial.println(samples);

  scale.begin(HX_DT_PIN, HX_SCK_PIN);
  // scale.power_up();
  // scale.set_gain(128);
  // delay(2000);
  while (!scale.is_ready()) {
  }

  scale.set_scale();
  //scale.set_offset(0);
  scale.tare(samples); //Reset the scale to 0

  zeroFactor = scale.read_average(samples); //Get a baseline reading
  Serial.print("Zero factor: "); //This can be used to remove the need to tare the scale. Useful in permanent scale projects.
  Serial.println(zeroFactor);

  delay(5000);

  scale.tare(samples); //Reset the scale to 0

  delay(5000);
}

void loop() {
  scale.set_scale(calibrationFactor); //Adjust to calibration factor

  float units = scale.get_units(samples);
  float value = scale.get_value(samples);

  Serial.print("Reading ");
  Serial.print("units: ");
  Serial.print(units, 3);
  Serial.print(" kg");
  Serial.print(", value: ");
  Serial.print(value, 3);
  //  Serial.print(", average: ");
  //  Serial.print(scale.read_average(samples));
  Serial.print(", calibration_factor: ");
  Serial.print(calibrationFactor);
  Serial.print(", zeroFactor: ");
  Serial.println(zeroFactor);

  bt.print("units: ");
  bt.print(units, 3);
  bt.print(" kg");
  bt.print(", value: ");
  bt.println(value, 3);

  if (Serial.available())
  {
    char temp = Serial.read();
    if (temp == 'a')
      calibrationFactor += 10;
    else if (temp == 'z') {
      calibrationFactor -= 10;
    }
    else if (temp == 't') {
      scale.tare(samples);
      zeroFactor = scale.read_average(samples);
      Serial.println("Scale tared.");
    }
  }

  if (bt.available())
  {
    char temp = bt.read();
    if (temp == 'a')
      calibrationFactor += 10;
    else if (temp == 'z') {
      calibrationFactor -= 10;
    }
    else if (temp == 't') {
      scale.tare(samples);
      zeroFactor = scale.read_average(samples);
      bt.println("Scale tared.");
    }
  }

  // delay(2000);
}
