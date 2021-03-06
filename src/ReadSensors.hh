// ReadSensors.hh

#ifndef READ_SENSORS
#define READ_SENSORS

#include "Arduino.h"

// Note: different than Arduino
const int MAX_ANALOG_IN = 4095;
const int MIN_ANALOG_IN = 0;

// CURRENT SENSORS
// varies linearly with DC in
const int NUM_CURRENT_SENSORS = 6;
const int MAX_CURRENT_IN = 33;

// Should be pins > 31 when WiFi on
const int CURRENT_SENSOR_VIN = 37;
const int MUX_SELECT_VOUT[] = {36, 39, 34};
const int POT_PINS[] = {13, 12, 14, 27, 26, 25};
// [0] is LSb and [2] is MSb
uint8_t muxSelects[3] = {};


void inline updateMuxSelect() {
	digitalWrite(MUX_SELECT_VOUT[0], muxSelects[0]);
	digitalWrite(MUX_SELECT_VOUT[1], muxSelects[1]);
	digitalWrite(MUX_SELECT_VOUT[2], muxSelects[2]);
}

/* 
 * After each call to updateMuxSelect() we now have routed a current sensor
 * read-out signal to CURRENT_SENSOR_VIN pin
 */
uint8_t currentNum = 0;
void readCurrentVal(int* a) {
	updateMuxSelect();
	a[currentNum] = analogRead(CURRENT_SENSOR_VIN);
	currentNum++;
	if (currentNum == 5) { currentNum = 0; }
}

/**
 * Inputs are routed to a 3x8 multiplexer.
 * After each call to updateMuxSelect() we now have routed a current sensor
 * read-out signal to CURRENT_SENSOR_VIN pins
 */
void inline getSensorData(int currentSensors[NUM_CURRENT_SENSORS], int pots[NUM_CURRENT_SENSORS])
{
	// signal 000
	muxSelects[0] = muxSelects[1] = muxSelects[2] = 0;
	readCurrentVal(currentSensors);

	// signal 001
	muxSelects[2] = 1;
	readCurrentVal(currentSensors);

	// signal 010
	muxSelects[1] = 1;
	muxSelects[2] = 0;
	readCurrentVal(currentSensors);

	// signal 011
	muxSelects[2] = 1;
	readCurrentVal(currentSensors);

	// signal 100
	muxSelects[0] = 1;
	muxSelects[1] = muxSelects[2] = 0;
	readCurrentVal(currentSensors);

	// signal 101
	muxSelects[2] = 1;
	readCurrentVal(currentSensors);
	
	// map to a number between 0 and 4095 to 0->33 Amps
	for (int i = 0; i < NUM_CURRENT_SENSORS; i++)
	{
		pots[i] = analogRead(POT_PINS[i]);
		map(pots[i], MIN_ANALOG_IN, MAX_ANALOG_IN, MIN_ANALOG_IN, MAX_CURRENT_IN);
	}
}

#endif
