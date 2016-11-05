#include <Arduino.h>

#define FORWARD 1
#define REVERSE 0  



class dcMotors{
  private:
    int in1; //direction control pin 1
    int in2; //direction control pin 2
    int en; //PWM to control speed
    
    
    void motorForward();
    void motorReverse();

  public:
    //decontructor
    virtual ~dcMotors();
    //contructor
    dcMotors(int, int, int);
    
    //Motor direction is dependent on physical connections made
    //if motor direction is not as expected change how motorforward 
    //and motorReverse toggle values or switch physical connection
    void motorAction(int direction, int speed);
};




