#ifndef MeArm_h
#define MeArm_h

#include "lib/MeArmServo.h"
#include "lib/pcf8591.h"

#define MEARM_DEFAULT_BASE_PIN 16
#define MEARM_DEFAULT_LOWER_PIN 12
#define MEARM_DEFAULT_UPPER_PIN 14
#define MEARM_DEFAULT_GRIP_PIN 13

#define I2C_DATA 0
#define I2C_CLOCK 2
#define PCF8591_ADDRESS B1001000

#define JOYSTICK_THRESHOLD 0.3f

#define BASE_SERVO 0
#define LOWER_SERVO 1
#define GRIP_SERVO 2
#define UPPER_SERVO 3

class MeArm {
  public:
    MeArm();
    void begin();
    void loop();
    void setServoPins(uint8_t, uint8_t, uint8_t, uint8_t);
  private:
    long adcNextSample = 0;
    int basePin;
    int lowerPin;
    int upperPin;
    int gripPin;
    MeArmServo base;
    MeArmServo lower;
    MeArmServo upper;
    MeArmServo grip;
    void joystickControl();
    void setupServos();
    void moveServoByPercent(uint8_t servo, float percent);
};
    
#endif