#include <MIDI.h>
#include <Wire.h>

#define NUM 3
#define MIDI_CHAN 1
#define DEBUG 1

#define GESTURE_START 0
#define GESTURE_N 20
#define FINGER_START 20
#define FINGER_N 8

#define CONTROL_N 28

//sensor values
int16_t accel_x, accel_y, accel_z;
int fingers[FINGER_N];
int ir; //TODO

int bendPins[8] = {A0, A9, A8, A6, A7, A3, A2, A1};
int bendThresholds[8] = { 600, 850, 600, 790, 700, 800, 780, 690};

//control values
uint8_t controls[CONTROL_N];

//accel init
#define accel_module (0x53)    //ACCELEROMETER SETUP
byte values [6];
char output [512];


//Which mode we are in: 1 is gesture, the rest are up to 
int mode = 1;

void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once
  initFingers();
  initAccel();
  Serial.println("Init done");
  
}

void loop() {
  Serial.println("sart loop");
  // put your main code here, to run repeatedly:
  //READ
  readAccel();
  readFingers();
  Serial.println("Mode " + (String) mode);
  if (mode == 1){ // gesture mode
    // TODO: calibrate accel
    int xval = map(accel_x, 130, -130, 0, 127);
    int yval = map(accel_y, 130, -130, 0, 127);
    
    int contx = -1;
    int conty = -1;
    
    bool fingerStates[FINGER_N];
    
    for (int i = 0; i < FINGER_N; i++){
      fingerStates[i] = (fingers[i] > bendThresholds[i]);
      Serial.print(i);
      Serial.print(fingerStates[i]);
      Serial.print(" ");
    }
    Serial.println();

    // gesture name goes here
    if (fingerStates[2] && fingerStates[3] && fingerStates[4]){
      contx = 1;
    }
    // ADD NEW GESTURES HERE
    else if (false) {
      conty = 2;
    }
    
    if (contx != -1){
      sendControl(contx, xval);
    }
  }
  delay(100); // TODO: proper timing calibration
  while (usbMIDI.read()) ; // read and discard any incoming MIDI messages
}

void sendControl(uint8_t num, uint8_t val) {
  usbMIDI.sendControlChange(num, val, 1); 
  #ifdef DEBUG
  Serial.print("sending ");
  Serial.print(num);
  Serial.print(" ");
  Serial.print(val);
  Serial.println();
  #endif
}
  
void initAccel(){
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
   
   accel_x = (((int16_t)values[1]) << 8) | values [0];
   accel_y = (((int16_t)values[3]) << 8) | values [2];
   accel_z = (((int16_t)values[5]) << 8) | values [4];
}

void initFingers(){
    for(int i = 0; i< FINGER_N; i++){
      //pinMode(bendPins[i], INPUT_PULLUP);
    }
}
void readFingers(){
  for(int i = 0; i< FINGER_N; i++){
    fingers[i] = analogRead(bendPins[i]);
  }
}
