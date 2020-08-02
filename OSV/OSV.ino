#include <Servo.h>
#include <Enes100.h>
#include "ObstacleEnums.h"
#include "Ultrasonic.h"
#include "FireSensor.h"

Enes100 enes("Avatar", FIRE, 7, 2, 3); //2 is RX pin, 14 is TX pin, 355 is identifier pin, 3 is the fire identifier
Ultrasonic sonicLeft(5, 14);
Ultrasonic sonicRight(5, 15);
Ultrasonic sonicMiddle(5, 111); // dont know what 111 should be
FireSensor fireLeft(0);
FireSensor fireRight(1);
FireSensor fireMiddle(2);
Servo fanServo;

void setup() {
  initEngine();
  enes.println("going forward");
  forward(255, 20); // test to move forward all the way till the end of the arena. Will need to pick up osv and place back at starting area
  rotateToDegrees(90); // rotation test
  delay(3000);
  rotateToDegrees(0);
  delay(3000);
  rotateToDegrees(-90); //end rotation test

  if (enes.retrieveDestination()) {
    enes.println("Destination Recieved");
    enes.println("Navigation Starting");
    navigateTo(enes.destination.x, enes.destination.y);
  }
  // now need to begin fire site testing
  int flameCount = extinguishFires();
  enes.baseObjective(flameCount);

  //now need to remove wooden dowel
  //removeWood();

  enes.endMission();
}

void loop() {
// nothing to do here
}



// *************
// ENGINE CODE
// ************
int pwm_right = 11;
int pwm_left = 10;
int digital_left_1 = 8;
int digital_left_2 = 9;
int digital_right_1 = 12;
int digital_right_2 = 13;

void initEngine() {
  pinMode(pwm_left, OUTPUT);
  pinMode(pwm_right, OUTPUT);
  pinMode(digital_left_1, OUTPUT);
  pinMode(digital_left_2, OUTPUT);
  pinMode(digital_right_1, OUTPUT);
  pinMode(digital_right_2, OUTPUT);
}

void forward(int val, int sec) {
  //turn on motors
  digitalWrite(digital_left_1, LOW);
  digitalWrite(digital_left_2, HIGH);
  digitalWrite(digital_right_1, LOW);
  digitalWrite(digital_right_2, HIGH);
  //set speed
  if (val <= 255 && val > 0) {
    analogWrite(pwm_right, val);
    analogWrite(pwm_left, val);
  }
  if (sec != 0) {
    //allow motors to run for specified time
    delay(sec * 1000);
    //Turn off motors
    motorsOff();
  }
}

void backward(int val, int sec) {
  digitalWrite(digital_left_1, HIGH);
  digitalWrite(digital_left_2, LOW);
  digitalWrite(digital_right_1, HIGH);
  digitalWrite(digital_right_2, LOW);
  //set speed
  if (val <= 255 && val > 0) {
    analogWrite(pwm_right, val);
    analogWrite(pwm_left, val);
  }
  if (sec != 0) {
    //allow motors to run for specified time
    delay(sec * 1000);
    //Turn off motors
    motorsOff();
  }
}

void rotateLeft(int val, int sec) {
  digitalWrite(digital_left_1, HIGH);
  digitalWrite(digital_left_2, LOW);
  digitalWrite(digital_right_1, LOW);
  digitalWrite(digital_right_2, HIGH);
  //set speed
  if (val <= 255 && val > 0) {
    analogWrite(pwm_right, val);
    analogWrite(pwm_left, val);
  }
  if (sec != 0) {
    //allow motors to run for specified time
    delay(sec * 1000);
    //Turn off motors
    motorsOff();
  }
}


void rotateRight(int val, int sec) {
  digitalWrite(digital_right_1, HIGH);
  digitalWrite(digital_right_2, LOW);
  digitalWrite(digital_left_1, LOW);
  digitalWrite(digital_left_2, HIGH);
  //set speed
  if (val <= 255 && val > 0) {
    analogWrite(pwm_right, val);
    analogWrite(pwm_left, val);
  }
  if (sec != 0) {
    //allow motors to run for specified time
    delay(sec * 1000);
    //Turn off motors
    motorsOff();
  }
}

void runThisLong(int sec) {
  delay(sec * 1000);
  //unsigned long currentMillis = millis();
  //unsigned long target = millis() + (sec * 1000);
  //while (target > currentMillis) {
  //while loop that will wait out the time

}

void motorsOff() {
  digitalWrite(digital_left_1, LOW);
  digitalWrite(digital_left_2, LOW);
  digitalWrite(digital_right_1, LOW);
  digitalWrite(digital_right_2, LOW);
}

// ****************
// END ENGINE CODE
// ****************




// **********************
// NAVIGATION SYSTEM CODE
// ***********************

void navigateTo(int x, int y) {
  enes.println("Now navigating to ");
  enes.println(x);
  enes.println(", ");
  enes.println(y);

  int tolerance = 10; // This will be used to determine how accurate we need the osv to get in terms of matching coordinates to postition.
  //First we will align the proper y coordinate before passing over the rocks.

  // if osv is on left side of arena in comparison to destination
  boolean inPosition = false; // boolean to know when to exit while loop. True means the osv is ready to preced to phase 2
  while (enes.updateLocation() && !inPosition) { //once the osv is in position in terms of y coordinate it will exit this while loop
    if (y < enes.location.y) { // if osv is on  side of arena in comparison to destination
      rotateToDegrees(-90);
      boolean inPosition2 = false; // another variable to know when to exit while loop
      while (enes.updateLocation() && !inPosition2) {
        forward(255, 0);
        if (y > enes.location.y - tolerance && y < enes.location.y + tolerance) {
          inPosition2 = true; //when y coordinate is reached exit while loop
        }
      }

      // if osv is on right side of arena in comparison to destination
    } else if (y > enes.location.y) {
      rotateToDegrees(90);
      boolean inPosition2 = false; // another variable to know when to exit while loop
      while (enes.updateLocation() && !inPosition2) {
        forward(255, 0);
        if (y < enes.location.y + tolerance && y > enes.location.y - tolerance) {
          inPosition2 = true; //when y coordinate is reached exit while loop
        }
      }
    }
    // this does a final check to ensure osv is in position. If it isn't the while loop will restart and the osv will re-attempt its past steps
    if (y < enes.location.y + tolerance && y > enes.location.y - tolerance) {
      rotateToDegrees(0); // orientate forward
      inPosition = true; //causes an exit of the while loop initiating phase 2
    }
  }
  enes.println("y coordinate reached, now navigating towards x coordinate");


  //phase 2, moving over rocks and towards the destination. x coordinate should be aligned
  inPosition = false;
  while (enes.updateLocation() && !inPosition) {
    if (whatsAhead() == CLEAR) { //idicating clear roads ahead NOTE: THE SONIC NUMBERS NEED TESTING
      forward(255, 1);
    }
    else if (whatsAhead() == RIGHT) { //idicating obstacle on right NOTE: THE SONIC NUMBERS NEED TESTING
      rotateToDegrees(90);
      forward(255, 1);
      rotateToDegrees(0);
    }
    else if (whatsAhead() == LEFT) { //idicating obstacle on left NOTE: THE SONIC NUMBERS NEED TESTING
      rotateToDegrees(-90);
      forward(255, 1);
      rotateToDegrees(0);
    }
    if (x > enes.location.x - tolerance && x < enes.location.x + tolerance) {
      inPosition = true;
    }
  }
  enes.println("y coordinate reached, now recalibrating x coordinate");

  //phase 3 --- making sure y coordinate is good again
  inPosition = false;
  int bigTolerance = 20; //need a bigger tolerance because of the size of the fire site
  while (enes.updateLocation() && !inPosition) {
    if (y < enes.location.y) { // if osv is on  side of arena in comparison to destination
      rotateToDegrees(-90);
      boolean inPosition2 = false; // another variable to know when to exit while loop
      while (enes.updateLocation() && !inPosition2) {
        forward(255, 0);
        if (y > enes.location.y - bigTolerance && y < enes.location.y + bigTolerance) {
          inPosition2 = true; //when y coordinate is reached exit while loop
        }
      }

      // if osv is on right side of arena in comparison to destination
    } else if (y > enes.location.y) {
      rotateToDegrees(90);
      boolean inPosition2 = false; // another variable to know when to exit while loop
      while (enes.updateLocation() && !inPosition2) {
        forward(255, 0);
        if (y < enes.location.y + bigTolerance && y > enes.location.y - bigTolerance) {
          inPosition2 = true; //when y coordinate is reached exit while loop
        }

        if (y < enes.location.y + bigTolerance && y > enes.location.y - bigTolerance
            && x < enes.location.x - bigTolerance && x > enes.location.x + bigTolerance) {
          inPosition = true;
        }
      }

      enes.println("Destination Reached");
      enes.navigated();
    }
  }
}

void rotateToDegrees(int deg) {
  int tolerance = 10;
  const float Pi = 3.14159;
  float rad = deg * (Pi / 180);
  float destination = convertPositive(rad);
  if(enes.updateLocation()) {
    enes.println("have location, about to rotate to degrees")
  }
  float pos = convertPositive(enes.location.theta);
  float result = destination - pos;
  if (result < 0) { //normalize the angle in reference to 0
    result = result + 2 * Pi;
  }
  if (result > 2 * Pi) {
    result = result - 2 * Pi;
  }
  if (result < Pi) {
    rotateLeft(255, 0);
  } else if (result > Pi) {
    rotateRight(255, 0);
  }

  boolean inPosition = false;
  while (enes.updateLocation() && !inPosition) {
    if (rad < enes.location.theta - tolerance
        || rad > enes.location.theta - tolerance) {
      motorsOff();
      inPosition = true;
    }
  }
}


Obstacle whatsAhead() {
  if (sonicLeft.distance() > 100 && sonicRight.distance() > 100) { //idicating clear roads ahead NOTE: THE SONIC NUMBERS NEED TESTING
    return CLEAR;
  }
  if (sonicLeft.distance() > 100 && sonicRight.distance() < 100) { //idicating obstacle on right NOTE: THE SONIC NUMBERS NEED TESTING
    return RIGHT;
  }
  if (sonicLeft.distance() < 100 && sonicRight.distance() > 100) { //idicating obstacle on left NOTE: THE SONIC NUMBERS NEED TESTING
    return LEFT;
  }
  return CONFUSED;
}


float convertPositive(float n) {
  const float Pi = 3.14159;
  if (n < 0) { // converts the theta location to a value from 0 to 2pi instead of having negatives
    float destination = (2 * Pi) - enes.location.theta;
    return destination;
  } else {
    float destination = enes.location.theta;
    return destination;
  }
}


// **************************
// END NAVIGATION SYSTEM CODE
// ***************************



// *************************
// ULTRASONIC SENSOR CODE
// **************************

/* provided simply for reference to library

  Ultrasonic::Ultrasonic(int trig, int echo) {
  initializePins(trig, echo);
  _trig = trig;
  _echo = echo;
  }
  ;

  Ultrasonic::Ultrasonic() {
  }
  ;

  void Ultrasonic::initializePins(int trig, int echo) {
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  }
  ;

  int Ultrasonic::distance() {
  // Clears the trigPin
  digitalWrite(_trig, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(_trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(_trig, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  int duration = pulseIn(_echo, HIGH);
  // Calculating the distance
  int distance = duration * 0.034 / 2;
  return distance;
  }
  ;

  void Ultrasonic::waitThisLong(int milli) {
  unsigned long currentMillis = millis();
  unsigned long target = millis() + milli;
  while (target > currentMillis) {
    //while loop that will wait out the time
  }
  }
  ;

*/


// *************************
// END ULTRASONIC SENSOR CODE
// **************************

// *********************
// EXTINGUISH FIRE CODE
// *********************

int extinguishFires() {
  fanServo.attach(6);
  initMosfet(7);
  int flameCount = 1; //Start at one because the middle flame will always be lit.



  if (fireMiddle.fireValue() > 500) {
    putOutFire();
  }
}

void putOutFire() { // This should only be called if the middle fire sensor has the fire value above the desired value
  //turn on mosfet which will turn on fan
  switchOn();
  //rotate servo
  boolean fireOut = false;
  while (!fireOut) {
    int pos = 0;
    for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
      // in steps of 1 degree
      fanServo.write(pos);              // tell servo to go to position in variable 'pos'
      delay(15);                       // waits 15ms for the servo to reach the position
    }
    for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
      fanServo.write(pos);              // tell servo to go to position in variable 'pos'
      delay(15);                       // waits 15ms for the servo to reach the position
    }
    if (fireMiddle.fireValue() < 500) { // If the fire has been extinguished this should be true
      fireOut = true;
    }
  }
  // now turn off mosfet
  switchOff();
}




// *************************
// END EXTINGUISH FIRE CODE
// *************************



// ****************
// FIRE SENSOR CODE
// ****************

/* provided simply for reference to library

  FireSensor::FireSensor(int pin) {
  _pin = pin;
  initializePin(pin);
  };

  void FireSensor::initializePin(int pin) {
  pinMode(pin, OUTPUT);
  };

  int FireSensor::fireValue(){
  int sensorValue = analogRead(_pin);
  return sensorValue;
  };

*/


// *********************
// END FIRE SENSOR CODE
// *********************



// ************
// MOSFET CODE
// ************

void initMosfet(int pin) {
  pinMode(pin, OUTPUT);
}

void switchOn() {
  digitalWrite(7, HIGH);
}

void switchOff() {
  digitalWrite(7, LOW);
}

// *********************
// END MOSFET CODE
// *********************
