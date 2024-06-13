/*
   @file: avrg_main.cpp
   @update: 2024/05/23
   @brief: Actuate solenoids for an air vortex ring generator, mounted on a gimbal platform
            Sensors: Rotary Encoers (CYTE1100)
            Acutators: 12v, 500mA solenoids, mg996r servos
            Power: 12V, 8A for solenoids; 5V, 3A for servo motors
*/

#include <Servo.h>

/******************************* global vars, objects ****************************************/
const int SOL_1       = 13;   //digital output pins to drive solenoids
const int SOL_2       = 12;         
const int SOL_3       = 11;
const int SOL_4       = 10;

const int PAN_KNOBA   = 3;   //Rotary encoder inputs | pan: x-axis & tilt: y-axis
const int PAN_KNOBB   = 4;
const int TILT_KNOBA  = 5;
const int TILT_KNOBB  = 6;
const int BUTTON_PIN  = 7;    //push button to actuate solenoids

const int PAN_SERVO   = 8;    //servo pwm output pins | pan: x-axis & tilt: y-axis
const int TILT_SERVO  = 9;

const int SERVO_STEP  = 3;    //number of degree's to increase servo position for every 1 increment of encoder

int panPrevState;             //pan knob's previous state of encoder
int tiltPrevState;            //tilt knob's previous state of encoder
int panPosServo   = 0;        //pan position of servo motor (0-180)
int tiltPosServo  = 0;        //tilt position of servo motor (0-180)
int panPrevServo  = 0;        //previous pan position 
int tiltPrevServo = 0;        //previous tilt position

Servo panServo;               //servo object declaration
Servo tiltServo;

/******************************* func protos ****************************************/
void getPosition(int, int, int*, int*);
void zeroServos();
void zeroSolenoids();
void driveSolenoids(int, int);

/******************************* void setup ****************************************/
void setup() {
  Serial.begin(9600);               //begin serial comms
  while(!Serial);

  pinMode(SOL_1, OUTPUT);           //set up solenoids
  pinMode(SOL_2, OUTPUT);
  pinMode(SOL_3, OUTPUT);
  pinMode(SOL_4, OUTPUT);
  
  pinMode(BUTTON_PIN, INPUT_PULLUP);//set up push button
  
  pinMode(PAN_KNOBA, INPUT_PULLUP);          //set up joystick
  pinMode(TILT_KNOBA, INPUT_PULLUP);
  pinMode(PAN_KNOBB, INPUT_PULLUP); 
  pinMode(TILT_KNOBB, INPUT_PULLUP);
  
  panServo.attach(PAN_SERVO);       //set up servo motors 
  tiltServo.attach(TILT_SERVO);
  delay(200);                

  zeroSolenoids();                  //set the position of servos to face forward
  zeroServos();                     //set the solenoids to deactivated state
  
  panPrevState = digitalRead(PAN_KNOBA);
  tiltPrevState = digitalRead(TILT_KNOBA);
  
  Serial.println("All actuators are zeroed");
  Serial.println("Ready to use gimbal");
  Serial.println("Ready to operate.");
}

/******************************* void loop ****************************************/
void loop() {
  //get position of pan/tilt rotary encoders
  getPosition(PAN_KNOBA, PAN_KNOBB, &panPosServo, &panPrevState);
  getPosition(TILT_KNOBA, TILT_KNOBB, &tiltPosServo, &tiltPrevState);

  //drive servo motors
  panServo.write(panPosServo);        
  tiltServo.write(tiltPosServo);

  //push button to project vortex ring using solenoids
  if( digitalRead(BUTTON_PIN) == LOW ){
    driveSolenoids(300, 100);  
  }

  //uncomment to get serial output of commanded servo position
//  if(panPosServo != panPrevServo || tiltPosServo != tiltPrevServo){
//    Serial.print("Pan: "); Serial.print(panPosServo); 
//    Serial.print(" "); 
//    Serial.print("Tilt: "); Serial.println(tiltPosServo); 
//  }
//  panPrevServo = panPosServo;
//  tiltPrevServo = tiltPosServo;

}

/******************************* user-defined functions ****************************************/
void zeroServos(){
  /* zeros position of servos to face straight forward*/
  panServo.write(0);
  tiltServo.write(0);
  delay(100);
}

void zeroSolenoids(){
  /* drives all solenoids to a deactivated state */
  digitalWrite(SOL_1, LOW);
  digitalWrite(SOL_2, LOW);
  digitalWrite(SOL_3, LOW);
  digitalWrite(SOL_4, LOW);
  delay(100);
}

void getPosition(int pinA, int pinB, int *counter, int *lastState){
  /* reads rotary encoder and increments/decrements the position counter between 0 and 180*/
  int currState = digitalRead(pinA);
  
  if( currState != *lastState ){
    if( digitalRead(pinB) != currState ){
      (*counter)+= SERVO_STEP;
    }
    else{
      (*counter)-= SERVO_STEP;
    }
    if( (*counter) >= 170 ){
      (*counter) = 170;
    }
    if( (*counter) <= 0 ){
      (*counter) = 0;
    }
  }
  
  *lastState = currState;
}

void driveSolenoids(int t_high, int t_low) {
  /* activate solenoid linear actuators for t_high ms, then deactivating */
  digitalWrite(SOL_1, HIGH);
  digitalWrite(SOL_2, HIGH);
  digitalWrite(SOL_3, HIGH);
  digitalWrite(SOL_4, HIGH);
  delay(t_high);
  digitalWrite(SOL_1, LOW);
  digitalWrite(SOL_2, LOW);
  digitalWrite(SOL_3, LOW);
  digitalWrite(SOL_4, LOW);
  delay(t_low);
}
