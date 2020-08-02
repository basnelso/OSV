/*
 * Ultrasonic.cpp
 *
 *  Created on: Mar 24, 2018
 *      Author: bs404
 */

#include "Ultrasonic.h"

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

