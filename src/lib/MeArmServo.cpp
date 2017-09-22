#include "MeArmServo.h"

MeArmServo::MeArmServo(){}

void MeArmServo::begin(int pin, int min, int max, int offset){
  _servo.attach(pin);
  readyTime = 0;
  maxAngle = max;
  minAngle = min;
  offsetAngle = offset;
}

void MeArmServo::moveToCentre(){
  setCurrentAngle(minAngle + (maxAngle - minAngle)/2);
}

void MeArmServo::updateServo(){
  /*
  var newPulseWidth = calculatePulseWidth(currentAngle);
  var diff = Math.abs(lastPulseWidth - newPulseWidth);
  lastPulseWidth = newPulseWidth;
  var duration = (diff/1900) * 500
  pin.servoWrite(newPulseWidth);
  if(cb) setTimeout(cb, duration);
  _servo.write
  */
}

void MeArmServo::setCurrentAngle(float angle){
  int diff = abs(angle - currentAngle);
  if(angle < minAngle){
    currentAngle = minAngle;
  }else if(angle > maxAngle){
    currentAngle = maxAngle;
  }else{
    currentAngle = angle;
  }
  updateServo();
  readyTime = millis() + (diff * 2.5); //approx 2.5ms / degree
  _servo.write(currentAngle);
}

float MeArmServo::getCurrentAngle(){
  return currentAngle - offsetAngle;
}

void MeArmServo::moveToAngle(float angle){
  setCurrentAngle(angle + offsetAngle);
}

void MeArmServo::moveByDegrees(float angle){
  setCurrentAngle(currentAngle + angle);
}

void MeArmServo::moveByPercent(float percent){
  setCurrentAngle(currentAngle + (((maxAngle - minAngle) / 100.0 ) * percent));
}

bool MeArmServo::ready(){
  return millis() >= readyTime;
}