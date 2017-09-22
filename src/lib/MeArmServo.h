#ifndef MeArmServo_h
#define MeArmServo_h

#include <Servo.h>

class MeArmServo{
  public:
    MeArmServo();
    void begin(int, int, int, int);
    void moveToCentre();
    void moveToAngle(float);
    void moveByDegrees(float);
    void moveByPercent(float);
    bool ready();
    float getCurrentAngle();
  private:
    Servo _servo;
    float currentAngle;
    int maxAngle;
    int minAngle;
    int offsetAngle;
    long readyTime;
    void updateServo();
    void setCurrentAngle(float);
};
    
#endif
