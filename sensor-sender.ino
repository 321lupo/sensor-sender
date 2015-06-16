#include <MIDI.h>
#include<Wire.h>

#define NUM 30
#define MIDI_CHAN 1
//sensor values

uint8_t current_sensor[NUM];
uint8_t prev_sensor[NUM];

//accel init
#define accel_module (0x53)    //ACCELEROMETER SETUP
byte values [6] ;
char output [512];

int16_t x,y,z;

void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
  for(int i = 0; i < NUM; i++){
    current_sensor[i] = 0;
    prev_sensor[i] = 0;
  }
  
  Wire.begin();
  Wire.beginTransmission(accel_module);
  Wire.write(0x2D);
  Wire.write(0);
  Wire.endTransmission();
  Wire.beginTransmission(accel_module);
  Wire.write(0x2D);
  Wire.write(16);
  Wire.endTransmission();
  Wire.beginTransmission(accel_module);
  Wire.write(0x2D);
  Wire.write(8);
  Wire.endTransmission();
}

void loop() {
  // put your main code here, to run repeatedly:
  //READ
  readAccel();
  
  
  //map accel
  current_sensor[2] = map(y, -130, 130, 0, 127);
  
  //SEND SENSORS
  for(uint8_t i = 0; i < NUM; i++){
    if (current_sensor[i] != prev_sensor[i]){
      usbMIDI.sendControlChange(i, current_sensor[i], MIDI_CHAN);
      prev_sensor[i] = current_sensor[i];
      Serial.print("sending ");
      Serial.print(i);
      Serial.print(" ");
      Serial.print(current_sensor[i]);
      Serial.println();
    }
  }
  delay(100);
  while (usbMIDI.read()) ; // read and discard any incoming MIDI messages
}

void readAccel() {
int16_t xyzregister = 0x32;

   Wire.beginTransmission(accel_module);
   Wire.write(xyzregister);
   Wire.endTransmission();
   Wire.beginTransmission(accel_module);
   Wire.write(xyzregister);
   Wire.endTransmission();
   Wire.beginTransmission(accel_module);
   Wire.requestFrom(accel_module, 6);
   int16_t i = 0;
   while(Wire.available()){
     values[i] = Wire.read();
     i++;
   }
   Wire.endTransmission();
   
   x = (((int16_t)values[1]) << 8) | values [0];
   y = (((int16_t)values[3]) << 8) | values [2];
   z = (((int16_t)values[5]) << 8) | values [4];
}
