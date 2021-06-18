/*

 Arduino pin 
 5 -> HX711 DT
 6 -> HX711 SCK
 5V -> HX711 VCC
 GND -> HX711 GND
 
*/

#include "HX711.h"

static const int HX_DT_PIN = 5;
static const int HX_SCK_PIN = 6;

HX711 scale;

float calibrationFactor = -29220; //-29220 worked for my 200 kg max scale setup
long zeroFactor = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("HX711 calibration");
  Serial.println("Remove all weight from scale");
  Serial.println("After readings begin, place known weight on scale");
  Serial.println("Press a to increase calibration factor");
  Serial.println("Press z to decrease calibration factor");
  Serial.println("Press t to tare the scale");

  scale.begin(HX_DT_PIN, HX_SCK_PIN);
  scale.set_scale();
  scale.tare(); //Reset the scale to 0

  zeroFactor = scale.read_average(); //Get a baseline reading
  Serial.print("Zero factor: "); //This can be used to remove the need to tare the scale. Useful in permanent scale projects.
  Serial.println(zeroFactor);
}

void loop() {

  scale.set_scale(calibrationFactor); //Adjust to this calibration factor

  Serial.print("Reading: ");
  Serial.print(scale.get_units(), 2);
  Serial.print(" kg");
  Serial.print(" calibration_factor: ");
  Serial.print(calibrationFactor);
  Serial.print(" zeroFactor: ");
  Serial.println(zeroFactor);

  if(Serial.available())
  {
    char temp = Serial.read();
    if(temp == 'a')
      calibrationFactor += 10;
    else if(temp == 'z')
      calibrationFactor -= 10;
    else if(temp == 't')
      scale.tare();
      zeroFactor = scale.read_average(); //Get a baseline reading
      Serial.print("Zero factor: "); //This can be used to remove the need to tare the scale. Useful in permanent scale projects.
      Serial.println(zeroFactor);
  }
}
