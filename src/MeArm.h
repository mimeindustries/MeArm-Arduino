#ifndef __MeArm_h__
#define __MeArm_h__

#include "lib/MeArmServo.h"
#include "lib/pcf8591.h"
#include "lib/Discovery.h"
#include "lwip/inet.h"
#include "espconn.h"
#include <ESP8266WiFi.h>
#include <Marceau.h>

#define MEARM_DEFAULT_BASE_PIN 16
#define MEARM_DEFAULT_LOWER_PIN 12
#define MEARM_DEFAULT_UPPER_PIN 14
#define MEARM_DEFAULT_GRIP_PIN 13

#define I2C_DATA 0u
#define I2C_CLOCK 2u
#define PCF8591_ADDRESS B1001000

#define JOYSTICK_THRESHOLD 0.3f

#define BASE_SERVO 0
#define LOWER_SERVO 1
#define GRIP_SERVO 2
#define UPPER_SERVO 3

#define VERSION "2.0.0"

static void _moveJointsTo(ArduinoJson::JsonObject &input, ArduinoJson::JsonObject &output);
static void _openGrip(ArduinoJson::JsonObject &input, ArduinoJson::JsonObject &output);
static void _closeGrip(ArduinoJson::JsonObject &input, ArduinoJson::JsonObject &output);
static void _moveBaseTo(ArduinoJson::JsonObject &input, ArduinoJson::JsonObject &output);
static void _moveLowerTo(ArduinoJson::JsonObject &input, ArduinoJson::JsonObject &output);
static void _moveUpperTo(ArduinoJson::JsonObject &input, ArduinoJson::JsonObject &output);
static void _moveGripTo(ArduinoJson::JsonObject &input, ArduinoJson::JsonObject &output);
static void _getServoState(ArduinoJson::JsonObject &input, ArduinoJson::JsonObject &output);
static void _version(ArduinoJson::JsonObject &input, ArduinoJson::JsonObject &output);
static void _empty(ArduinoJson::JsonObject &input, ArduinoJson::JsonObject &output);

class MeArm {
  public:
    MeArm();
    void begin();
    void loop();
    void setServoPins(uint8_t, uint8_t, uint8_t, uint8_t);
    static MeArm getMeArmInstance();
    MeArmServo base;
    MeArmServo lower;
    MeArmServo upper;
    MeArmServo grip;
    void getServoState(ArduinoJson::JsonObject &input, ArduinoJson::JsonObject &output);
    static MeArm * mainInstance;
  private:
    long adcNextSample = 0;
    int basePin;
    int lowerPin;
    int upperPin;
    int gripPin;
    long nextDiscovery = 0;
    void joystickControl();
    void checkDone();
    void setupServos();
    void moveServoByPercent(uint8_t servo, float percent);
    void generateAPName(char * name);
    void sendDiscovery();
};
    
#endif