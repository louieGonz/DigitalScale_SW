#include <Time.h>
#include <EEPROM.h>
#include <math.h>

#include "HX711.h"
#include "dcMotors.h"


#define FALSE 0
#define TRUE 1
#define WAITING 1
#define CALIBRATION 2
#define WEIGHING 3


/* 
FUNCTION PROTOTYPES
  Summary: will check weight to passed in value, 
  Arguement: the weight to be compared to 

  Return = if the same weight
            1
         else 
            0
*/

unsigned char weight_verify(float comparingWeight);


/*
 * Summary: Will check current weight, quickly
 * Arguement: NONE
 * Return: ! iff weight is zero, -1 if not zero
 */
unsigned char is_weight_zero()
