#include "HX711.h"
#include <Time.h>

#define FALSE 0
#define TRUE 1

// HX711.DOUT  - pin #A1
// HX711.PD_SCK - pin #A0

HX711 scale(A1, A0);    // parameter "gain" is ommited; the default value 128 is used by the library





void setup() {
  Serial.begin(38400);
  Serial.println("HX711 Demo");

  scale.set_scale();
  scale.tare();

  calibrate();

}

void loop() {

 
  //Serial.print("time: \t"); Serial.print(millis()); Serial.print("\t\t");

  //unsigned long data_o = scale.read();
  
  //Serial.print("Reading in Hex: \t");Serial.print(scale.raw_dout, HEX);Serial.print("\t");
  //Serial.print("Reading In Dec: \t");Serial.println(data_o);
  
  Serial.print(i++); Serial.print(")\t");
  Serial.print("one reading:\t");
  Serial.print(scale.get_units(), 1);
  Serial.print("\t| average:\t");
  Serial.println(scale.get_units(10), 1);
  

  //scale.power_down();             // put the ADC in sleep mode
  //delay(5000);
  //scale.power_up();
}

void calibrate(){
  char inputx=' ';
  float calibWeight=0.0;
  char calibFlag=' ';
  int wait_for_veri=TRUE;
  int wait_for_input=TRUE;
  char input_ver= ' ';

//  Serial.println("Enter the weight in grams of calibration weight: \t");
//  //wait until get input and is verified
//  while(wait_for_input){
//
//    if(Serial.available() > 0) 
//      calibWeight = Serial.parseFloat(); //read calibration weight
//
//    else if(calibWeight>1 && calibWeight<2000){
//        Serial.print("verify that weight entered is : \t");
//        Serial.println((float)calibWeight);
//
//        while(wait_for_veri==TRUE){
//          if( Serial.available()>0){
//            input_ver = Serial.read();
//          }
//          if(input_ver == 'y'){
//            wait_for_input=FALSE;
//            wait_for_veri=FALSE;  
//          }
//          else if(input_ver=='n'){
//            wait_for_input=TRUE; //exit while loop wait_for_input
//            wait_for_veri=TRUE;   //go while(wait for veri) to try entering again
//          }
//          
//      }//end of wait_for_input
//    }//end of if
//
//  }//end of wait_for_veri
//      
//  Serial.println("place the weight on the scale, then press 'x' ");
//  //wait until they place the weight then
//
//  char input = ' ';
//  while(input!='x'){
//    input = Serial.read();
//  }
//  

  scale.set_scale();
  scale.tare();

  float xx=0;
  
  char input = ' ';
  Serial.println("Place the weight no the scale and press 'x' ");
  while(input!='x'){
    input = Serial.read();
  }
  

  xx=scale.get_units(10);

  Serial.print("get units is be4 calib \t");Serial.println((float)xx);
  //Serial.print("calib weight is \t");Serial.println(calibWeight);

  //Serial.print("scaling factor is  \t");Serial.println(xx/(int)calibWeight);
  input = ' ';
  Serial.println("Remove the weight then press 'x'");
  while(input!='x'){
    input = Serial.read();
  }
 
  scale.set_scale(xx/55);
  scale.tare();


  Serial.println("scale is calibrated!");

 
}//end of function
