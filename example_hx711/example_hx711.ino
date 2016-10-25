#include <Time.h>
#include <EEPROM.h>
#include <math.h>

#include <Stepper.h>

#include "HX711.h"
#include "dcMotors.h"


#define FALSE 0
#define TRUE 1
#define BUF_QUICK_CHECK 2 //used to do quick check on weight 


enum States{
  NONE,
  WAITING,
  CALIBRATION,
  WEIGHING,
  TRANSFERRING,
  RECHECK,
  CLEAR_SCALE
};


enum Events{
  idle,            //0
  weight_on,       //1
  weight_change,   //2
  button_on,       //3
  button_off,      //4
  button_calibrate,//5
  berry_on        //6
};


// HX711.DOUT  - pin #A1
// HX711.PD_SCK - pin #A0

char * currScalingFactor = "777"; //Nothing until init

HX711 scale(A1, A0);    // parameter "gain" is ommited; the default value 128 is used by the library
dcMotors motorA(7,8,9);
const int stepsPerRevolution = 200;  // change this to fit the number of steps per revolution
                                     // for your motor

// initialize the stepper library on pins 8 through 11:
//Stepper myStepper(stepsPerRevolution, 8,9,10,11);     


//Global Declaration
//
static volatile unsigned char state = idle; //starting state is idle
static volatile unsigned char event = 0;
static float currWeight=0;
static float prevWeight=0;
static float v_weight=0;
static unsigned char weighing_hold = FALSE;

SensorData scaleAvg, scaleAvg2;
unsigned char serial_in= ' ';
int diff = 0;
unsigned char byte0;
unsigned char byte1;
unsigned char byte2;
unsigned char byte3;
int wait_for_response;
char inputt;


Events event_s;
States state_s;

float scalingFactor;
int motorflag=0;
unsigned long start_time=0,end_time=0;



/*****************************************************************************/

void setup() {
  Serial.begin(38400);
  // set the speed at 60 rpm:
  //myStepper.setSpeed(60);
  Serial.println("Scale Demo");
  Serial.println("Do you want to calibrate 'y' or 'n");
  inputt = ' ';
  wait_for_response = TRUE;

  

  toCalibrate();

  currWeight = round(scale.get_units(10));
  
  //starting state is
  //state = TRANSFERRING;
  state = WEIGHING;

}


char input_i = ' ';
/*******************************************************************************************/
//Infinite loop 
/*******************************************************************************************/
void loop() {
  //Event Checking
  start_time = millis();
   
   //Update the Scale Reading/Weight
   currWeight = ( scale.get_units(RUNNING_MED_BUF_LEN) );
   ( currWeight + 0.5f <= ceil(currWeight) ) ? currWeight=floor(currWeight) : currWeight=ceil(currWeight);
      
   
   //Event Checker
   if(prevWeight!=currWeight){
    //Serial.println("Event Weight On");
    event = weight_on;
   }
   
   /** Check for keyboard input *******************************************************/
   if( Serial.available() ){
    serial_in = Serial.read();
    //Calibrate button
    if(serial_in == 'x'){
      Serial.println("Calibrate event");
      scale.calibrate();
      //event = event_s.BUTTON_CALIBRATE;
    }
   }
  /********************************************************/ 
  
   //state Machine
  switch (state){
    /********************************************************/
    case WAITING:
    {
       Serial.println("~~~~Now Waiting~~~~~");
       if(event == weight_on){
           //Serial.println("~~~~~To Weight On state");
           state = WEIGHING;
       }
       else if(event == button_calibrate){
           Serial.println("~~~~~To Calibrate Scale ");
           state = CALIBRATION;
       }
       break;
    }
    /********************************************************/
    case CALIBRATION:
    {
       Serial.println("In Calibration State");
       scale.calibrate(); //recalibrating the scale
       state = WAITING;
       break;
    }
    /********************************************************/
    case WEIGHING:
    {
       
      //initial state waiting for weight data
      //when data found send to VERIFY
      //get new wight and checks if same as currWeight
      
          
      //start_time = millis();
       //currWeight = ( scale.get_units(RUNNING_MED_BUF_LEN) );
       //currWeight + 0.5f <= ceil(currWeight) ? currWeight=floor(currWeight) : currWeight=ceil(currWeight);
       //Serial.println(start_time);
       //Serial.println(currWeight);
       //motorA.motorAction(FORWARD,0);//stop the motor
       //only print if the weight has changed
       //if(currWeight != prevWeight){
       //end_time = millis();
       //Serial.print("in g = \t");
       //Serial.println(currWeight,1);
       
       //Serial.print("delta time = ");
       //Serial.println(end_time - start_time);
         //prevWeight = currWeight;
       //}
       //else Serial.println("No Weight Change");
       //prevWeight=currWeight;

       if(weight_verify(currWeight)){
          state = CLEAR_SCALE;
          Serial.print("Valid: in g = \t");
          Serial.println(currWeight,1);
       }
       else{
          //need to check weight again
          //reWeigh = currWeight; //store the value 
          //if(weight_verify(reWeight))
          state = RECHECK;
       }  
       
       break;
     
    }
    /********************************************************/
    case RECHECK:
    {
      //just move between weighing and recheck until valid weight
      if(weight_verify(currWeight)){
        //flag to wait until next berry for new weights
        weighing_hold = TRUE;
        state = CLEAR_SCALE;
        
      }
      
      else 
        state = WEIGHING;

      break;
      
    }
    /********************************************************/
//    case TRANSFERRING:
//    {
//       //Serial.println("Belt Moving");
//       motorA.motorAction(FORWARD,255);
//       if(event == weight_on){
//          start_time = millis();
//          state = WEIGHING;
//          //Serial.println("Transferring to Weighing"); 
//       }
//         //state = WEIGHING;
//        
//       break;
//    }

    /********************************************************/
    case CLEAR_SCALE:
    {

      //
      //if(emergency_stop)
      //  state=STOP;
      //move berries of the scale
      //until weight goes back to zero
      //this is blocking
      //Serial.print("Valid: in g = \t");
      //Serial.println(currWeight,1);
      // step one revolution  in one direction:
      //Serial.println("clockwise");
      //myStepper.step(stepsPerRevolution);
      motorA.motorAction(FORWARD,255);
      //Serial.println("Weight is Valid");
      //end_time = millis();
      //Serial.print("delta time = ");
      //Serial.println(end_time - start_time);
      //Serial.println();
      
     
      if(is_weight_zero()){
        state = WAITING;
        end_time = millis();
        Serial.print("delta time = ");
        Serial.println(end_time - start_time);
        Serial.println("Scale is now Clear");
        Serial.println();
        
        motorA.motorAction(FORWARD,0);
        
      }
        
      
        //motorA.motorAction(FORWARD,0);
   
      break;
   }

   
    break;
  }//end of switch statement
  event=idle;//clear the event var
  prevWeight = currWeight;
}
/*******************************************************************************************/
//Occurs when new data comes to hardware UART Rx 
/*******************************************************************************************/
void serialEvent(){
  Serial.println("Serial Event");
   if( Serial.available() ){
    
    serial_in = Serial.read();
    
    //Calibrate button
    if(serial_in == 'x'){
      Serial.println("Calibrate event");
      scale.calibrate();
      //event = event_s.BUTTON_CALIBRATE;
    }
    
    //TARE button
    else if(serial_in == 't'){
      Serial.println("Tare event");
      scale.tare(10);
    }
    
    //ON button
    else if(serial_in == 'm'){
      Serial.println("Berry event");
      event = berry_on; 
    }
  }
}

/*******************************************************************************************/
//Helper Functions 
/*******************************************************************************************/
//toCalibrate

void toCalibrate(){
  //wait till get input
  while (wait_for_response) {
    inputt = Serial.read();
    if (inputt == 'y') {
      scale.calibrate();
      wait_for_response = FALSE; //will break out of for loop
    }
    else if (inputt == 'n') {
      char * i;
      EEPROM.write(0, *currScalingFactor);
      Serial.println(EEPROM.read(0));
      wait_for_response = FALSE; //will break out of for loop
    }
  }
  Serial.println("Exiting Calibration");
}


unsigned char weight_verify(float comparingWeight){
  
  float newWeight;
  
  newWeight = ( scale.get_units(RUNNING_MED_BUF_LEN) );
  newWeight + 0.5f <= ceil(newWeight) ? currWeight=floor(newWeight) : currWeight=ceil(newWeight);
  if(newWeight==comparingWeight)
    return 1;
  else 
    return -1;
  
   
}

float get_weight(){
  float returnWeight;
  returnWeight = ( scale.get_units(RUNNING_MED_BUF_LEN) );
  returnWeight + 0.5f <= ceil(returnWeight) ? returnWeight=floor(returnWeight) : returnWeight=ceil(returnWeight); 

  return returnWeight;
}

//will check is scale has nothing ie is zero
unsigned char is_weight_zero(){

  float temp = scale.get_units(BUF_QUICK_CHECK);
  if(temp<=0.0){
    Serial.print("zero_weight_check val = ");
    Serial.println(temp);
    return TRUE;
  }
  else return FALSE;
}


