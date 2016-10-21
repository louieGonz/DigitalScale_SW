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