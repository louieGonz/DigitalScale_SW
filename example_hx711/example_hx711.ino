#include "HX711.h"
#include <Time.h>
#include <EEPROM.h>
#include <math.h>


#define FALSE 0
#define TRUE 1
#define WAITING 1
#define CALIBRATION 2
#define WEIGHING 3


// HX711.DOUT  - pin #A1
// HX711.PD_SCK - pin #A0

char * currScalingFactor = "777"; //Nothing until init

HX711 scale(A1, A0);    // parameter "gain" is ommited; the default value 128 is used by the library


//function prototype



long data0=0,data1=0,data2=0;

long currRead,prevRead;
 SensorData scaleAvg, scaleAvg2;



long currRead = 0, prevRead = 0,data0=0,data1=0,data2=0;

int diff = 0;

struct events{
  
  unsigned char WEIGHT_ON = 1;
  unsigned char START_BUTTON=2;
  unsigned char NOTHING=3;
  unsigned char BUTTON_CALIBRATE=4;
  
};


struct states {
  
  unsigned char idle = 1;
  unsigned char START_BUTTON=2;
  
};


unsigned char byte0;
unsigned char byte1;
unsigned char byte2;
unsigned char byte3;
int wait_for_response;
char inputt;
float xx;

float ooh;
float currWeight=0;
float prevWeight=0;


float scalingFactor;

  struct events event_s;
  struct states state_s;

  unsigned char state = state_s.idle; //starting state is idle
  unsigned char event = 0;
void setup() {
  Serial.begin(38400);

  Serial.println("Scale Demo");
  Serial.println("Do you want to calibrate 'y' or 'n");


  inputt = ' ';
  wait_for_response = TRUE;


  byte0 = EEPROM.read(0);
  byte1 = EEPROM.read(1);
  byte2 = EEPROM.read(2);
  byte3 = EEPROM.read(3);

  scalingFactor = byte3 << 24 | byte2 << 16 | byte1 << 8 | byte0;
  //
  //  Serial.print("current scaling factor is \t");
  //  Serial.println((float)scalingFactor);



  //wait till get input
  while (wait_for_response) {
    inputt = Serial.read();
    if (inputt == 'y') {

      calibrate();
      wait_for_response = FALSE; //will break out of for loop
    }
    else if (inputt == 'n') {

      char * i;
      EEPROM.write(0, *currScalingFactor);


      Serial.println(EEPROM.read(0));

      wait_for_response = FALSE; //will break out of for loop
    }
  }
  //else starting measuring
  state = WEIGHING;
 


  inputt = ' ';
  wait_for_response = TRUE;


  byte0 = EEPROM.read(0);
  byte1 = EEPROM.read(1);
  byte2 = EEPROM.read(2);
  byte3 = EEPROM.read(3);

  scalingFactor = byte3 << 24 | byte2 << 16 | byte1 << 8 | byte0;
  //
  //  Serial.print("current scaling factor is \t");
  //  Serial.println((float)scalingFactor);



  //wait till get input
  while (wait_for_response) {
    inputt = Serial.read();
    if (inputt == 'y') {

      calibrate();
      wait_for_response = FALSE; //will break out of for loop
    }
    else if (inputt == 'n') {

      char * i;
      EEPROM.write(0, *currScalingFactor);


      Serial.println(EEPROM.read(0));

      wait_for_response = FALSE; //will break out of for loop
    }
  }
  //else starting measuring




}


/*****************************************************************************/
void loop() {
  unsigned char event = 0;
  char input_i = ' ';
  
  prevRead = currRead;
  currRead = scale.read();
<<<<<<< HEAD

  //scaleAvg = scale.get_units(5);
  //delay(30);
  //scaleAvg2 = scale.get_units(5);

//  if(abs(scaleAvg.runningMedian- scaleAvg2.runningMedian) < .5){
//    float avg = (scaleAvg.runningMedian + scaleAvg2.runningMedian) / 2;
//    Serial.print("The Item weighs");Serial.print(avg);Serial.println("~~~~~");
//    
//  }
  

  //Serial.print("~~");Serial.print(currRead);Serial.println("~~");
  
  

  //Serial.println(currRead);
  //diff = abs(currRead.runningAverage - prevRead.runningAverage);


  diff = abs(currRead - prevRead);

  
  //check for button push event
  if( Serial.available() ){
    if(Serial.read() == 'x')
      event = event_s.BUTTON_CALIBRATE;
  }

//  else if(diff>100){
//    event = event_s.WEIGHT_ON;
//  }
//  else
//    event = event_s.NOTHING;

  else if(diff>100){
    event = event_s.WEIGHT_ON;
  }
  else
    event = event_s.NOTHING;


  

  //state Machine

  switch (state){
    case WAITING:
    {
  //      if(event==events.START_BUTTON){
  //        next_state = Running;
  //      }
        if(event == event_s.WEIGHT_ON){
            Serial.println("~~~~~To Weight On state");
            state = WEIGHING;
        }
        else if(event == event_s.BUTTON_CALIBRATE){
            Serial.println("~~~~~To Calibrate Scale ");
            state = CALIBRATION;
        }
        break;
    }
    case CALIBRATION:
      Serial.println("In Calibration State");
      calibrate(); //recalibrating the scale
      state = WAITING;
      break;

    case WEIGHING:

      currWeight = round(scale.get_units(10));
      //Serial.println("In Weight on State");
      Serial.print("Current Weight in g = \t");
      Serial.println(currWeight,1);
      //state = WAITING;
      if(currWeight < 1.0 && prevWeight > 1.0){
        //state = WAITING;
        Serial.println("weight taken off the scale");
        //semi-calibrate for next weighing 
        scale.set_scale(ooh);
        scale.tare();
      }  
      else
        state = WEIGHING;
      delay(330);
      prevWeight = currWeight;

      Serial.println("In Weight on State");
      Serial.print("The Weight in g = \t");
      Serial.println(scale.get_units(10),1);
      state = WAITING;

      break;
  
  }//end of switch statement

  

//  prevRead = currRead;
//
//  currRead = scale.read();
//
//  Serial.print(currRead); Serial.print("\t");
//  Serial.print(prevRead); Serial.print("\t");
//  diff = abs(currRead - prevRead);
//  Serial.println(diff);
//
//
//  if (diff > 100) {
//    //Serial.println("~~~~~~~~~~~~~~~~~~~something was placed on the scale~~~~~~~~~~~~~~~~~");
//    delay(100);
//    Serial.print("~~~~~~~~The weight is \t");
//    Serial.println(scale.get_units(3),1);
//
//  }
//  Serial.print("one reading:\t");
//  Serial.print(scale.get_units(), 1);
//  Serial.print("\t| average of 10:\t");
//  Serial.println(scale.get_units(10), 1);

  //scale.power_down();             // put the ADC in sleep mode
  //delay(5000);
  //scale.power_up();
}

void calibrate() {
  char inputx = ' ';
  float calibWeight = 0.0;
  char calibFlag = ' ';
  int wait_for_veri = TRUE;
  int wait_for_input = TRUE;
  char input_ver = ' ';

  //Serial.println("Enter the weight in grams of calibration weight: \t");
  //wait until get input and is verified
//  while (wait_for_input) {
//
//    if (Serial.available() > 0)
//      calibWeight = Serial.parseFloat(); //read calibration weight
//
//    else if (calibWeight > 1 && calibWeight < 2000) {
//      Serial.print("verify that weight entered is : \t");
//      Serial.println((float)calibWeight);
//
//      while (wait_for_veri == TRUE) {
//        if ( Serial.available() > 0) {
//          input_ver = Serial.read();
//        }
//        if (input_ver == 'y') {
//          wait_for_input = FALSE;
//          wait_for_veri = FALSE;
//        }
//        else if (input_ver == 'n') {
//          wait_for_input = TRUE; //exit while loop wait_for_input
//          wait_for_veri = TRUE; //go while(wait for veri) to try entering again
//        }
//
//      }//end of wait_for_input
//    }//end of if
//
//  }//end of wait_for_veri


  //calling with no paramets or weights
  scale.set_scale();
  scale.tare();

  Serial.println("place the weight on the scale, then press 'x' ");
  //wait until they place the weight then

  char input = ' ';
  while (input != 'x') {
    input = Serial.read();
  }


  xx = scale.get_units(10);

  Serial.print("Get Units value = \t");Serial.println(xx);


  input = ' ';
  Serial.println("Remove the weight then press 'x'");
  while (input != 'x') {
    input = Serial.read();
  }


  ooh = xx / 5.0;

  Serial.print("current scaling factor is \t");
  Serial.println(ooh);

  float ooh = xx / 5;

  Serial.print("current scaling factor is \t");
  Serial.println((float) ooh);


  scale.set_scale(ooh);
  scale.tare();


  Serial.println("scale is calibrated!");


}//end of function
