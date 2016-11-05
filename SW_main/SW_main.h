#include <Time.h>
#include <EEPROM.h>
#include <math.h>

#include "HX711.h"


#define ADDR 0
#define FALSE 0
#define TRUE 1



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
unsigned char is_weight_zero();


/*
 * Summary: simplifies the calls needed to get a weight reading
 * Arguement: the amount of samples to be taken
 * Return: weight on the scale as a float
 */
float get_weight(int amount);




/*
 * Summary: Will read flash, for previous calibration
 * Arguement: NONE
 * Return: the float value used in calibration
 */
float readFlash();
