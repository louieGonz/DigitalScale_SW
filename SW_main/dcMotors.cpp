#include "dcMotors.h"

//constructor
dcMotors::dcMotors(int in1,int in2, int pwmEn){
  in1 = in1;
  in2 = in2;
  en = pwmEn;
  pinMode(in1,OUTPUT);
  pinMode(in2,OUTPUT);
  pinMode(pwmEn,OUTPUT);
}


//deconstructor
dcMotors::~dcMotors(){
  
}

void dcMotors::motorAction(int direction, int speed){
   if(direction == FORWARD){
      motorForward();
    }
    else if(direction == REVERSE){
      motorReverse();
    }

    analogWrite(en, speed); 
}

//Private function prototypes
void dcMotors::motorForward(){
  digitalWrite(in1, 0);
  digitalWrite(in2, 1);
}  

void dcMotors::motorReverse(){
  digitalWrite(in1, 1);
  digitalWrite(in2, 0);
}

