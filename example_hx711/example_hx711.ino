#include "HX711.h"
#include <Time.h>

// HX711.DOUT  - pin #A1
// HX711.PD_SCK - pin #A0

HX711 scale(A1, A0);    // parameter "gain" is ommited; the default value 128 is used by the library

void setup() {
  Serial.begin(38400);
  Serial.println("HX711 Demo");

  scale.set_scale();          // this value is obtained by calibrating the scale with known weights; see the README for details
  scale.tare(); 
  
  int done=0;

  Serial.println("Place a weight and press (a) when placed");

  char inputx = ' ';
  while(done!=1){
    if(Serial.available() > 0){
      inputx = Serial.read();
        if( inputx = 'a')
          done=1;
        }
  }
  float offset_val=0.0;
  float scale_val = 0.0;
  
  Serial.print("The Offset value is: \t\t");
  offset_val = scale.get_units(10);
  Serial.println(offset_val);


  inputx = ' ';
  done=0;
  Serial.println("remove the weight then press p");
  while(done!=1){
    if(Serial.available() > 0){
      inputx = Serial.read();
        if( inputx = 'p')
          done=1;
        }
  }


  scale_val = offset_val / 53; // 53 is weight of known item

  scale.set_scale(float(scale_val));
  scale.tare(); 

 
  Serial.println("Readings:");
}

void loop() {

 
  //Serial.print("time: \t"); Serial.print(millis()); Serial.print("\t\t");

  //unsigned long data_o = scale.read();
  
  //Serial.print("Reading in Hex: \t");Serial.print(scale.raw_dout, HEX);Serial.print("\t");
  //Serial.print("Reading In Dec: \t");Serial.println(data_o);
  
  
  Serial.print("one reading:\t");
  Serial.print(scale.get_units(), 1);
  Serial.print("\t| average:\t");
  Serial.println(scale.get_units(10), 1);

  //scale.power_down();             // put the ADC in sleep mode
  //delay(5000);
  //scale.power_up();
}
