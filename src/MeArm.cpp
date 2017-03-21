#include "MeArm.h"

PCF8591 adc(I2C_DATA, I2C_CLOCK, PCF8591_ADDRESS);

MeArm::MeArm(){
  setServoPins(MEARM_DEFAULT_BASE_PIN, MEARM_DEFAULT_LOWER_PIN, MEARM_DEFAULT_UPPER_PIN, MEARM_DEFAULT_GRIP_PIN);
}

void MeArm::begin(){
  setupServos();
  base.moveToCentre();
  lower.moveToCentre();
  upper.moveToCentre();
  grip.moveToCentre();
}

void MeArm::loop(){
  joystickControl();
}

void MeArm::joystickControl(){
  uint8_t rawValues[4];
  float scaledValue;
  if(adcNextSample < millis()){
    adcNextSample = millis() + 20;
    adc.readSensors(rawValues);
    for(uint8_t i=0; i< 4; i++){
      scaledValue = (rawValues[i] - 128) / 128.0;
      if(scaledValue > JOYSTICK_THRESHOLD || scaledValue < -JOYSTICK_THRESHOLD){
        // remove the threshold
        scaledValue -= (scaledValue < 0 ? -1 : 1) * JOYSTICK_THRESHOLD;
        //move the servo
        moveServoByPercent(i, scaledValue * 2);
      }
    }
  }
}

void MeArm::moveServoByPercent(uint8_t servo, float percent){
  switch(servo){
    case BASE_SERVO:
      base.moveByPercent(percent);
      break;
    case LOWER_SERVO:
      lower.moveByPercent(percent);
      break;
    case UPPER_SERVO:
      upper.moveByPercent(percent);
      break;
    case GRIP_SERVO:
      grip.moveByPercent(percent);
      break;
  }
}

void MeArm::setServoPins(uint8_t _basePin, uint8_t _lowerPin, uint8_t _upperPin, uint8_t _gripPin){
  basePin = _basePin;
  lowerPin = _lowerPin;
  upperPin = _upperPin;
  gripPin = _gripPin;
}

void MeArm::setupServos(){
  base.begin(basePin, 0, 180, -90);
  lower.begin(lowerPin, 0, 90, 0);
  upper.begin(upperPin, 0, 135, 0);
  grip.begin(gripPin, 90, 180, 0);
}
