#include <Arduino.h>
#include <HX711.h>

HX711::HX711(byte dout, byte pd_sck, byte gain) {
  PD_SCK  = pd_sck;
  DOUT  = dout;

  raw_dout = 0;

  pinMode(PD_SCK, OUTPUT);
  pinMode(DOUT, INPUT);

  set_gain(gain);
}

HX711::~HX711() {

}

void HX711::calibrate(){
  char inputx=' ';
  float calibWeight=0.0;
  char calibFlag=' ';

  serial.println("Enter the weight in grams of calibration weight: \t");
  //wait until get input and is verified
  while(wait_for_veri){

    if(serial.available > 0) 
      calibWeight = serial.read(); //read calibration weight

    if(calibWeight>1 && calibWeight<2000){
        serial.print("verify that weight entered is : \t");
        serial.println(calibWeight);

        while(wait_for_input==0){
          if(serial.available>0)
            input_ver = serial.read();

          if(input_ver == 'y'){
            wait_for_input=FALSE;
            wait_for_veri=FALSE;  
          }
          else if(input_ver=='n'){
            wait_for_input=FALSE; //exit while loop wait_for_input
            wait_for_veri=TRUE;   //go while(wait for veri) to try entering again
          }
          else
            serial.println("enter 'y' if correct or 'n' if NOT correct");
        }      
    }  
  }


  serial.println("place the weight on the scale, then press 'x' ");
  //wait until they place the weight then
  while(input!='x'){
    inputx = serial.read();
  }
  

  set_scale();
  set_tare();

  long xx=0;

  xx=scale.get_units(30);


  scale.set(xx / calibWeight);
  scale.tare();


  serial.println("scale is calibrated!");

}
    

bool HX711::is_ready() {
  return digitalRead(DOUT) == LOW;
}

void HX711::set_gain(byte gain) {
  switch (gain) {
    case 128:   // channel A, gain factor 128
      GAIN = 1;
      break;
    case 64:    // channel A, gain factor 64
      GAIN = 3;
      break;
    case 32:    // channel B, gain factor 32
      GAIN = 2;
      break;
  }

  digitalWrite(PD_SCK, LOW);
  read();
}




long HX711::read() {
  // wait for the chip to become ready
  while (!is_ready());

    unsigned long value = 0;
    byte data[3] = { 0 };
    byte filler = 0x00;


  // pulse the clock pin 24 times to read the data
    data[2] = shiftIn(DOUT, PD_SCK, MSBFIRST); //shiftIn does 1 byte, so call 3x
    data[1] = shiftIn(DOUT, PD_SCK, MSBFIRST);
    data[0] = shiftIn(DOUT, PD_SCK, MSBFIRST);

  // set the channel and the gain factor for the next reading using the clock pin
  for (unsigned int i = 0; i < GAIN; i++) {
    digitalWrite(PD_SCK, HIGH);
    digitalWrite(PD_SCK, LOW);
  }


   raw_dout = ( static_cast<unsigned long> (0x00) << 24
            | static_cast<unsigned long>(data[2]) << 16
            | static_cast<unsigned long>(data[1]) << 8
            | static_cast<unsigned long>(data[0]) );

    // Datasheet indicates the value is returned as a two's complement value
    // Flip all the bits
    data[2] = ~data[2];
    data[1] = ~data[1];
    data[0] = ~data[0];

    // Replicate the most significant bit to pad out a 32-bit signed integer
    //if data is negative num
    if ( data[2] & 0x80 ) {
        filler = 0xFF;
    // else if data is 0b0111_1111_1111_111 ie highest positive value
    } else if ((0x7F == data[2]) && (0xFF == data[1]) && (0xFF == data[0])) {
        filler = 0xFF;
    //else data is 
    } else {
        filler = 0x00;
    }

    // Construct a 32-bit signed integer
    value = ( static_cast<unsigned long>(filler) << 24
            | static_cast<unsigned long>(data[2]) << 16
            | static_cast<unsigned long>(data[1]) << 8
            | static_cast<unsigned long>(data[0]) );

    // ... and add 1
    return static_cast<long>(++value);
}

long HX711::read_average(byte times) {
  long sum = 0;
  for (byte i = 0; i < times; i++) {
    sum += read();
  }
  return sum / times;
}

double HX711::get_value(byte times) {
  return read_average(times) - OFFSET;
}

float HX711::get_units(byte times) {
  return get_value(times) / SCALE;
}

void HX711::tare(byte times) {
  double sum = read_average(times);
  set_offset(sum);
}

void HX711::set_scale(float scale) {
  SCALE = scale;
}

float HX711::get_scale() {
  return SCALE;
}

void HX711::set_offset(long offset) {
  OFFSET = offset;
}

long HX711::get_offset() {
  return OFFSET;
}

void HX711::power_down() {
  digitalWrite(PD_SCK, LOW);
  digitalWrite(PD_SCK, HIGH);
}

void HX711::power_up() {
  digitalWrite(PD_SCK, LOW);
}
