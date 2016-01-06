// myAttitudeIndicatorFloat
// Simulator Cockpit Panel - Attitude Indicator
// Copyright (C) 2016 Michael Melchior
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

// arduino servo library
#include <Servo.h> 

const int pitchPin = 9;
const int rollPin = 10;

// two servos to be used, one for pitch, the other for roll indication 
Servo pitchServo, rollServo;

// serial communication handling
// each message starts with the letter 'h'
char headerChar;
// followed by an identifier
int headerInt;
// and then the values for pitch and roll
// the updated value will be stored to avoid unnecessary settings to the servos
float pitchFloat, oldPitchFloat;
int rollFloat, oldRollFloat;
 
void setup()
{
  // start the serial communication at 9600 baud
  Serial.begin(9600);
  
  // notify the Servo library to use pins 9 and 10 for the servos
  pitchServo.attach(pitchPin);
  rollServo.attach(rollPin);

  // start with both servos centered
  // by setting the values
  pitchFloat = 90;
  rollFloat = 90;
  oldPitchFloat = 90;
  oldRollFloat = 90;

  // and then setting the servos
  pitchServo.write(pitchFloat);
  rollServo.write(rollFloat);
} // void setup()

void loop() 
{
  // wait until we receive serial communication
  if (Serial.available() > 0) {
	// read each char
    headerChar = Serial.read();
    
	// proceed only if the read char is the initiating 'h'
    if (headerChar == 'h') {
      headerInt = Serial.parseInt();

	  // received a message for the attitude indicator
	  // this is necessary, since the serial comms is being shared with other instruments
      if (headerInt == 0) {

		// check for the delimiter
        if(Serial.read() != ';')
          return;

		// the pitch value is being sent with the factor of 100
		// add 90 to it, since the center of the motor is 90
        pitchFloat = (Serial.parseInt() / 100) + 90;

		// check for the delimiter
        if(Serial.read() != ';')
          return;
        
		// the roll value is being sent with the factor of 100
		// substract 90 from it, since the center of the motor is 90
        rollFloat = 90 - (Serial.parseInt() / 100);

		// due to some jitter of the motors at the lower most / upper most angle,
		// the travel is being limited from 10° to 170°
        if (pitchFloat > 170)
          pitchFloat = 170;
        if (pitchFloat < 10)
          pitchFloat = 10;
    
		// due to some jitter of the motors at the lower most / upper most angle,
		// the travel is being limited from 10° to 170°
        if (rollFloat > 170)
          rollFloat = 170;
        if (rollFloat < 10)
          rollFloat = 10;
  
		// to save some cycles, we only set the servo if the value is different from before
        if (pitchFloat != oldPitchFloat) {
          pitchServo.write(pitchFloat);
          oldPitchFloat = pitchFloat;
        }
  
		// to save some cycles, we only set the servo if the value is different from before
        if (rollFloat != oldRollFloat) {
          rollServo.write(rollFloat);
          oldRollFloat = rollFloat;
        }
      
      } // if (header == 0)
    } // if (header == 'h') {
  } // if (Serial.available() > 0) {
    
} // void loop()
