/*
 * FireSensor.cpp
 *
 *  Created on: Mar 24, 2018
 *      Author: bs404
 */

#include "FireSensor.h"

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

