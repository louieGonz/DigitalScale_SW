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
    virtual ~dcMotors();
    dcMotors(int, int, int);
    void motorAction(int direction, int speed);
};




