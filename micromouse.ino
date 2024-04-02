/** DO NOT CHANGE THE FOLLOWING DEFINITONS - From UKMARS MazeRunner GitHub **/
/** =============================================================================================================== **/
#if defined(__AVR_ATmega328__) || defined(__AVR_ATmega328P__)
#define __digitalPinToPortReg(P) (((P) <= 7) ? &PORTD : (((P) >= 8 && (P) <= 13) ? &PORTB : &PORTC))
#define __digitalPinToDDRReg(P) (((P) <= 7) ? &DDRD : (((P) >= 8 && (P) <= 13) ? &DDRB : &DDRC))
#define __digitalPinToPINReg(P) (((P) <= 7) ? &PIND : (((P) >= 8 && (P) <= 13) ? &PINB : &PINC))
#define __digitalPinToBit(P) (((P) <= 7) ? (P) : (((P) >= 8 && (P) <= 13) ? (P)-8 : (P)-14))

// general macros/defines
#if !defined(BIT_READ)
#define BIT_READ(value, bit) ((value) & (1UL << (bit)))
#endif
#if !defined(BIT_SET)
#define BIT_SET(value, bit) ((value) |= (1UL << (bit)))
#endif
#if !defined(BIT_CLEAR)
#define BIT_CLEAR(value, bit) ((value) &= ~(1UL << (bit)))
#endif
#if !defined(BIT_WRITE)
#define BIT_WRITE(value, bit, bitvalue) (bitvalue ? BIT_SET(value, bit) : BIT_CLEAR(value, bit))
#endif

#define fast_write_pin(P, V) BIT_WRITE(*__digitalPinToPortReg(P), __digitalPinToBit(P), (V));
#define fast_read_pin(P) ((int)(((BIT_READ(*__digitalPinToPINReg(P), __digitalPinToBit(P))) ? HIGH : LOW)))

#else
#define fast_write_pin(P, V) fast_write_pin(P, V)
#define fast_read_pin(P) digitalRead(P)
#endif

/** =============================================================================================================== **/

/** DEFINE OUR PINS AND WHICH COMPONENTS THEY ARE CONNECTED TO **/
/** _______________________________________________________________________________________________________________ **/
const int ENCODER_R_A = 3; // ENCODER RIGHT A (ticks first when motor forward)
const int ENCODER_R_B = 5; // ENCODER RIGHT B (ticks first when motor backward) 

const int ENCODER_L_A = 4; // ENCODER LEFT A (ticks first when motor forward)
const int ENCODER_L_B = 2; // ENCODER LEFT B (ticks first when motor backward)

const int SPEED_MOTOR_L = 9; // PWM MOTOR LEFT 
const int SPEED_MOTOR_R = 10; // PWM MOTOR RIGHT 

const int DIR_MOTOR_L = 7; // DIRECTION MOTOR LEFT 
const int DIR_MOTOR_R = 8; // DIRECTION MOTOR RIGHT 

//____________________________________________________________________________
const int EMITTERS = 12; // EMITTERS
const int RED_LED = 13; // RED LED AT H BRIDGE

const int INDICATOR_LED_R = 6; // INDICATOR LED RIGHT 
const int INDICATOR_LED_L = 11; // INDICATOR LED LEFT

// Phototransistors
const int RIGHT_SENSOR = A0;
const int LEFT_SENSOR = A2;
const int MIDDLE_SENSOR = A1;

//____________________________________________________________________________

// 4 Way switch and push button
const int DIP_SWITCH = A6; 
/** _______________________________________________________________________________________________________________ **/

/* GLOBAL VARIABLES */

volatile int rightEncoderPos = 0; // Counts for right encoder ticks
volatile int leftEncoderPos = 0; // Counts for left encoder ticks

// Variables to help us with our PID
int prevTime = 0;
int prevError;
int errorIntegral;
bool switchOn = false;

#include "maze.h"

/** INTERRUPT SERVICE ROUTINES FOR HANDLING ENCODER COUNTING USING STATE TABLE METHOD **/
void readEncoderLeft() {
  static uint8_t prevState = 0; 
  static uint8_t currState = 0; 
  static unsigned long lastTime = 0; 
  
  currState = (fast_read_pin(ENCODER_L_B) << 1) | fast_read_pin(ENCODER_L_A);
  
  unsigned long currentTime = micros();
  unsigned long deltaTime = currentTime - lastTime;
  lastTime = currentTime;
  
  // direction based on prev state
  uint8_t direction = (prevState << 2) | currState;
  switch(direction) {
    case 0b0001:
    case 0b0111:
    case 0b1110:
    case 0b1000:
      leftEncoderPos++;
      break;
    case 0b0010:
    case 0b1100:
    case 0b0101:
    case 0b1011:
      leftEncoderPos--;
      break;

    default:
      break;
  }

  prevState = currState;
}
void readEncoderRight() {
  static uint8_t prevState = 0; 
  static uint8_t currState = 0; 
  static unsigned long lastTime = 0; 
  
  currState = (fast_read_pin(ENCODER_R_B) << 1) | fast_read_pin(ENCODER_R_A);
  
  unsigned long currentTime = micros();
  unsigned long deltaTime = currentTime - lastTime;
  lastTime = currentTime;
  
  uint8_t direction = (prevState << 2) | currState;
  switch(direction) {
    case 0b0100:
    case 0b1010:
    case 0b0111:
    case 0b1001:
      rightEncoderPos++;
      break;
    case 0b1000:
    case 0b0110:
    case 0b1101:
    case 0b0011:
      rightEncoderPos--;
      break;

    default:
      break;
  }
  
  prevState = currState;
}

/** Function to set motor speed and direction for BOTH motors
    @params dir - can either be HIGH or LOW for clockwise / counter clockwise rotation
    @params speed - analogWrite() value between 0-255
**/
//==============================================================================================
void setMotor_r(int dir, int speed){
  analogWrite(SPEED_MOTOR_R, speed);
  
  if(dir == 1){
    fast_write_pin(DIR_MOTOR_R, LOW);
  } else if (dir == -1){
    fast_write_pin(DIR_MOTOR_R, HIGH);
  } else{
    analogWrite(SPEED_MOTOR_R, 0);
  }
}

void setMotor_l(int dir, int speed){
  analogWrite(SPEED_MOTOR_L, speed);
  
  if(dir == 1){
    fast_write_pin(DIR_MOTOR_L, HIGH);
  } else if (dir == -1){
    fast_write_pin(DIR_MOTOR_L, LOW);
  } else{
    analogWrite(SPEED_MOTOR_L, 0);
  }
}
//==============================================================================================

/** Function to make the robot travel for a certain amount of encoder ticks, calls upon setMotors at end
    @params dir - setPoint: The target value for how far we want to go (in encoder ticks)
    @params speed - analogWrite() value between 0-255
    @params kp - proportional gain, this is the main one you should be changing
    @params ki - intergral gain, use this for steady state errors
    @params kd - derivative gain, use this for overshoot and oscillation handling 
**/
void motorPID_r(int setPoint, float kp, float ki, float kd){
  int currentTime = micros();
  int deltaT = ((float)(currentTime - prevTime)) / 1.0e6; // time difference between ticks in seconds
  prevTime = currentTime; // update prevTime each loop 
  
  int error = setPoint - rightEncoderPos;
  int errorDerivative_r = (error - prevError_r) / deltaT;
  errorIntegral_r = errorIntegral_r + error*deltaT;

  float u = kp*error + ki*errorIntegral_r + kd*errorDerivative_r; 

  float speed = fabs(u); // Set a top speed
  if(speed > 150){
    speed = 150;
  }

  int dir = 1;
  if (u < 0) {
    dir = -1; // Move backward
  } else {
    dir = 1; // Move forward
  }

  setMotor_r(dir, speed);
  prevError_r = 0;
}

void motorPID_l(int setPoint, float kp, float ki, float kd){
  int currentTime = micros();
  int deltaT = ((float)(currentTime - prevTime)) / 1.0e6; // time difference between ticks in seconds
  prevTime = currentTime; // update prevTime each loop 
  
  int error = setPoint - leftEncoderPos;
  int errorDerivative_l = (error - prevError_l) / deltaT;
  errorIntegral_l = errorIntegral_l + error*deltaT;

  float u = kp*error + ki*errorIntegral_l + kd*errorDerivative_l; 

  float speed = fabs(u); // Set a top speed
  if(speed > 150){
    speed = 150;
  }

  int dir = 1;
  if (u < 0) {
    dir = -1; // Move backward
  } else {
    dir = 1; // Move forward
  }

  setMotor_l(dir, speed);
  prevError_l = 0;
}

//==============================================================================================
// Reset our encoder ticks
void resetCount(){
  rightEncoderPos = 0;
  leftEncoderPos = 0;
  setMotor_r(0, 0);
  setMotor_l(0, 0);
}


// Helper functions when moving
void goForward(){
  motorPID_r(forwardTarget, forwardPID[0]-0.0588, forwardPID[1], forwardPID[2]);
  motorPID_l(forwardTarget, forwardPID[0]+0.005, forwardPID[1], forwardPID[2]);
  while (leftEncoderPos < forwardTarget && rightEncoderPos < forwardTarget) {
    delay(30); 
  }
}

void turnRight(){
  motorPID_r(-rotateTarget, rotatePID[0], rotatePID[1], rotatePID[2]);
  motorPID_l(rotateTarget, rotatePID[0], rotatePID[1], rotatePID[2]);
  while (leftEncoderPos < rotateTarget && rightEncoderPos < rotateTarget) {
    delay(30); 
  }
}

void turnLeft(){
  motorPID_r(rotateTarget+5, rotatePID[0], rotatePID[1], rotatePID[2]);
  motorPID_l(-rotateTarget+5, rotatePID[0], rotatePID[1], rotatePID[2]);
  while (leftEncoderPos < rotateTarget && rightEncoderPos < rotateTarget) {
    delay(30); 
  }
}

void turnAround(){
  motorPID_r(rotateTarget*2, rotatePID[0], rotatePID[1], rotatePID[2]);
  motorPID_l(-rotateTarget*2, rotatePID[0], rotatePID[1], rotatePID[2]);
  while (leftEncoderPos < rotateTarget && rightEncoderPos < rotateTarget) {
    delay(30); 
  }
}
//==============================================================================================

void setup() {
  Serial.begin(9600);
  
  //_________________MOTORS AND ENCODERS________
  pinMode(ENCODER_R_A, INPUT_PULLUP);
  pinMode(ENCODER_R_B, INPUT_PULLUP);
  pinMode(ENCODER_L_A, INPUT_PULLUP);
  pinMode(ENCODER_L_B, INPUT_PULLUP);

  pinMode(SPEED_MOTOR_L, OUTPUT);
  pinMode(SPEED_MOTOR_R, OUTPUT);
  pinMode(DIR_MOTOR_L, OUTPUT);
  pinMode(DIR_MOTOR_R, OUTPUT);
  
  //________________LEDS_____________________
  pinMode(EMITTERS, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(INDICATOR_LED_R, OUTPUT);
  pinMode(INDICATOR_LED_L, OUTPUT);
  //_________________________________________

  pinMode(DIP_SWITCH, INPUT_PULLUP);

  //__________________________________________

  attachInterrupt(digitalPinToInterrupt(ENCODER_L_B), readEncoderLeft, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODER_R_A), readEncoderRight, CHANGE);
}

void loop() {
  // digitalWrite(EMITTERS, HIGH);
  // Serial.println(analogRead(RIGHT_SENSOR));  // 130 - 100
  // Serial.println(analogRead(LEFT_SENSOR));     // 130 - 110
  // Serial.println(analogRead(MIDDLE_SENSOR));     // 130 - 100

  digitalWrite(DIR_MOTOR_L, HIGH);
  analogWrite(SPEED_MOTOR_L, 180);

  digitalWrite(DIR_MOTOR_R, LOW);
  analogWrite(SPEED_MOTOR_R, 180);
  
  //setMotors(1, 180);
  //adjustSpeedBasedOnWallDistance(); 

}
