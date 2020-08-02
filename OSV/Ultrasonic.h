/*
 * Ultrasonic.h
 *
 *  Created on: Mar 24, 2018
 *      Author: bs404
 */

#ifndef ULTRASONIC_H_
#define ULTRASONIC_H_

#include <Arduino.h>

class Ultrasonic {
private:
	int _trig;
	int _echo;
	void initializePins(int trig, int echo);
	void waitThisLong(int milli);
public:
	Ultrasonic(int trig, int echo);
	Ultrasonic(); // default constructor
	int distance();
};

#endif /* ULTRASONIC_H_ */
