/*
 * FireSensor.h
 *
 *  Created on: Mar 24, 2018
 *      Author: bs404
 */

#ifndef FIRESENSOR_H_
#define FIRESENSOR_H_

#include <Arduino.h>

class FireSensor {
  private:
    int _pin;
    void initializePin(int pin);
  public:
    FireSensor(int pin);
    int fireValue();
};

#endif /* FIRESENSOR_H_ */
