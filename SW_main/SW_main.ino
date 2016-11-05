
#include <Time.h>
#include <EEPROM.h>
#include <math.h>

#include "HX711.h"
#include "dcMotors.h"
#include "SW_main.h"



#define BUF_QUICK_CHECK 2 //to perform a quick check on scale reading 


//defines states in system
enum States{
  NONE,
  WAITING,
  CALIBRATION,
  WEIGHING,
  TRANSFERRING,
  RECHECK,
  CLEAR_SCALE
};


//defines events in system
enum Events{
  idle,            //0
  weight_on,       //1
  weight_change,   //2
  button_on,       //3
  button_off,      //4
  button_calibrate,//5
  berry_on         //6
};

const byte interruptPin = 2; //digital pin 2 is enabled to external interrupts

// DOUT - pin #A1
// SCK - pin #A0
HX711 scale(A1, A0, 128);    // Note gain is set to 128 


//motorIn1 = #D7
//motorIn2 = #D8
//Enable = #D9 (pwm pin)
dcMotors motorA(7,8,9);


   
//Global Declaration
static float currWeight=0; //
static float prevWeight=0;
static unsigned char weighing_hold = FALSE; //flag indicating state of 

unsigned char serial_in= ' ';

int wait_for_response = TRUE;
char inputt = ' ';
static int state,event;


Events event_s;
States state_s;
unsigned long start_time=0,end_time=0;
float calib_val;
char input_i = ' ';


/*****************************************************************************/

void setup() {
  Serial.begin(38400);

  //attachInterrupt(2,IRCounter, HIGH);//interrupt responding to berry recieved
  
  //Timer1.initialize(5000000); //5s Oh Shit Timer.
  //Timer1.attachInterrupt(backUpTimer);
  
  #if defined(DEMO)
  
  Serial.println("Scale Demo");
  Serial.print("Last saved Calibration val = ");
  calib_val = readFlash();
  Serial.println(c_val);
  
  #endif

  
  Serial.println("Do you want to calibrate 'y' or 'n");
  
  while(!Serial.available()); //wait for user input

  inputt=Serial.read();
  if(inputt=='y')
    scale.calibrate();
  else{
    calib_val = readFlash();
    Serial.print("Calibration weight uses is: ");
    Serial.println(calib_val);
    
    //the scale now has a value for calibration
    scale.set_scale(calib_val);
    scale.tare();
    
  }
  
  currWeight = get_weight(10); //get a current weight before send control to state machine
  
  //set the starting state
  state = WEIGHING;

}
/*******************************************************************************************/
//ISR Timer1
/*******************************************************************************************/
void IRCounter(){
  

  
}


/*******************************************************************************************/
//Infinite loop 
/*******************************************************************************************/
void loop() {
  
  start_time = millis(); //debugging statement
   
   //Update the Scale Reading/Weight
   currWeight = get_weight(RUNNING_MED_BUF_LEN);
   
   /** Check for events ************************************************************/
   //Event Checker -- could be moved to timer 
   if(prevWeight!=currWeight && currWeight>0.0 ){
    event = weight_on;
   }
   
   //check for input events
   if( Serial.available() ){
    serial_in = Serial.read();
    //Calibrate button
    if(serial_in == 'x'){
      Serial.println("Calibrate event");
      scale.calibrate();
      //event = event_s.BUTTON_CALIBRATE;
    }
   }
  /**********************************************************************************/ 
  
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
       if(weight_verify(currWeight)){
          state = CLEAR_SCALE;
          Serial.print("Valid: in g = \t");
          Serial.println(currWeight,1);
       }
       else{
          //need to check weight again
          state = RECHECK;
       }  
       
       break;
     
    }
    /********************************************************/
    case RECHECK:
    {
      //just move between weighing and recheck until valid weight
      if(weight_verify(currWeight)){
        state = CLEAR_SCALE;
      }
      else 
        state = WEIGHING;

      break;
      
    }
    /********************************************************/
    //state that would control the belt movement
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
      
      
      motorA.motorAction(FORWARD,255);
      
     if(is_weight_zero()){
        state = WAITING;
        end_time = millis();
        Serial.print("delta time = ");
        Serial.println(end_time - start_time);
        Serial.println("Scale is now Clear");
        Serial.println();
        
        motorA.motorAction(FORWARD,0);
        
      }
      break;
   }

  }//end of switch statement
  
  
  event=idle;//clear the event var
  prevWeight = currWeight;
}


/*******************************************************************************************/
//Helper Functions -- Info at SW_main.h
/*******************************************************************************************/
unsigned char weight_verify(float comparingWeight){
  
  float newWeight;

  newWeight = get_weight(RUNNING_MED_BUF_LEN);
  
  if(newWeight==comparingWeight)
    return 1;
  else 
    return -1;
  
   
}


unsigned char is_weight_zero(){

  float temp = scale.get_units(BUF_QUICK_CHECK);
  
  Serial.print("zero_weight_check val = ");
  Serial.println(temp);
  
  if(temp<0.5f){
    Serial.print("zero_weight_check val = ");
    Serial.println(temp);
    return TRUE;
  }
  else return FALSE;
}


float readFlash(){

   float flo_val;
   unsigned char dataRx[sizeof(float)];
   
   dataRx[0]=EEPROM.read(ADDR); 
   dataRx[1]=EEPROM.read(ADDR+1); 
   dataRx[2]=EEPROM.read(ADDR+2); 
   dataRx[3]=EEPROM.read(ADDR+3);  
  
    memcpy(&flo_val,dataRx,sizeof(float));

    return flo_val;
}

float get_weight(int amount){
  
  float returnWeight;
  returnWeight = ( scale.get_units(amount) );
  returnWeight + 0.5f <= ceil(returnWeight) ? returnWeight=floor(returnWeight) : returnWeight=ceil(returnWeight); 

  return returnWeight;
}


