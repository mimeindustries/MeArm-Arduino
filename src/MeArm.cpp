#include "MeArm.h"

PCF8591 adc(I2C_DATA, I2C_CLOCK, PCF8591_ADDRESS);
Marceau<14> marcel;

MeArm *MeArm::mainInstance;

MeArm::MeArm(){
  mainInstance = this;
  setServoPins(MEARM_DEFAULT_BASE_PIN, MEARM_DEFAULT_LOWER_PIN, MEARM_DEFAULT_UPPER_PIN, MEARM_DEFAULT_GRIP_PIN);
  marcel.addCmd("moveJointsTo", _moveJointsTo, false);
  marcel.addCmd("moveBaseTo", _moveBaseTo, false);
  marcel.addCmd("moveLowerTo", _moveLowerTo, false);
  marcel.addCmd("moveUpperTo", _moveUpperTo, false);
  marcel.addCmd("moveGripTo", _moveGripTo, false);
  marcel.addCmd("openGrip", _openGrip, false);
  marcel.addCmd("closeGrip", _closeGrip, false);
  marcel.addCmd("getServoState", _getServoState, true);
  marcel.addCmd("version", _version, true);
  marcel.addCmd("pause", _empty, true);
  marcel.addCmd("resume", _empty, true);
  marcel.addCmd("stop", _empty, true);
#ifdef ESP8266
  marcel.addCmd("updateFirmware",   _updateFirmware,   true);
  marcel.addCmd("updateUI",         _updateUI,         true);
#endif //ESP8266
}

void MeArm::generateAPName(char * name){
  uint8_t mac[6];
  WiFi.softAPmacAddress(mac);
  sprintf(name, "MeArm-%02X%02X", mac[4], mac[5]);
}

void MeArm::begin(){
  static char defaultAPName[11];
  generateAPName(defaultAPName);
  setupServos();
  Serial.begin(230400);
  marcel.enableSerial(Serial);
  marcel.enableWifi();
  marcel.setHostname("local.mearm.com");
  marcel.setDefaultAPName(defaultAPName);
  marcel.begin();
  base.moveToCentre();
  lower.moveToCentre();
  upper.moveToCentre();
  grip.moveToCentre();
}

void MeArm::loop(){
  marcel.loop();
  joystickControl();
  sendDiscovery();
  checkDone();
  updateHandler();
}

void MeArm::joystickControl(){
  uint8_t rawValues[4];
  bool changed = false;
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
        changed = true;
      }
    }
  }
  if(changed){
    StaticJsonBuffer<500> outBuffer;
    JsonObject& output = outBuffer.createObject();
    JsonObject& msg = output.createNestedObject("msg");
    msg["base"] = base.getCurrentAngle();
    msg["lower"] = lower.getCurrentAngle();
    msg["upper"] = upper.getCurrentAngle();
    msg["grip"] = grip.getCurrentAngle();
    marcel.notify("servoChange", output);
  }
}

void MeArm::checkDone(){
  if(base.ready() && lower.ready() && upper.ready() && grip.ready()){
    marcel.cmdComplete();
  }
}

void MeArm::sendDiscovery(){
  if(nextDiscovery < millis()){
    if(marcel.wifi.online){
      send_discovery_request(WiFi.localIP(), marcel.settings.ap_ssid, "MeArm WiFi");
      nextDiscovery = millis() + 30000;
    }else{
      nextDiscovery = millis() + 1000;
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
  base.begin(basePin, 0, 180, 90);
  lower.begin(lowerPin, 0, 90, 0);
  upper.begin(upperPin, 0, 135, 0);
  grip.begin(gripPin, 90, 180, 90);
}

void MeArm::getServoState(ArduinoJson::JsonObject &input, ArduinoJson::JsonObject &output){
  JsonObject& msg = output.createNestedObject("msg");
  msg["base"] = base.getCurrentAngle();
  msg["lower"] = lower.getCurrentAngle();
  msg["upper"] = upper.getCurrentAngle();
  msg["grip"] = grip.getCurrentAngle();
}

#ifdef ESP8266
void MeArm::updateFirmware(){
  if(marcel.wifi.online){
    ESPhttpUpdate.rebootOnUpdate(true);
    if(ESPhttpUpdate.update("http://downloads.mime.co.uk/MeArm/WiFi/v1/mirobot-latest.bin", "") != HTTP_UPDATE_OK){
      Serial.println(ESPhttpUpdate.getLastErrorString());
    }
  }
}

void MeArm::updateUI(){
  if(marcel.wifi.online){
    ESPhttpUpdate.rebootOnUpdate(false);
    if(ESPhttpUpdate.updateSpiffs("http://downloads.mime.co.uk/MeArm/WiFi/v1/ui-latest.bin", "") != HTTP_UPDATE_OK){
      Serial.println(ESPhttpUpdate.getLastErrorString());
    }
  }
}

void MeArm::updateHandler(){
  if(_updateFWflag){
    _updateFWflag = false;
    updateFirmware();
  }
  if(_updateUIflag){
    _updateUIflag = false;
    updateUI();
  }
}
#endif //ESP8266

static void _moveJointsTo(ArduinoJson::JsonObject &input, ArduinoJson::JsonObject &output){
}

static void _openGrip(ArduinoJson::JsonObject &input, ArduinoJson::JsonObject &output){
  MeArm::mainInstance->grip.moveToAngle(0);
}

static void _closeGrip(ArduinoJson::JsonObject &input, ArduinoJson::JsonObject &output){
  MeArm::mainInstance->grip.moveToAngle(90);
}

static void _moveBaseTo(ArduinoJson::JsonObject &input, ArduinoJson::JsonObject &output){
  MeArm::mainInstance->base.moveToAngle(atoi(input["arg"].asString()));
}

static void _moveLowerTo(ArduinoJson::JsonObject &input, ArduinoJson::JsonObject &output){
  MeArm::mainInstance->lower.moveToAngle(atoi(input["arg"].asString()));
}

static void _moveUpperTo(ArduinoJson::JsonObject &input, ArduinoJson::JsonObject &output){
  MeArm::mainInstance->upper.moveToAngle(atoi(input["arg"].asString()));
}

static void _moveGripTo(ArduinoJson::JsonObject &input, ArduinoJson::JsonObject &output){
  MeArm::mainInstance->grip.moveToAngle(atoi(input["arg"].asString()));
}

static void _getServoState(ArduinoJson::JsonObject &input, ArduinoJson::JsonObject &output){
  MeArm::mainInstance->getServoState(input, output);
}

static void _version(ArduinoJson::JsonObject &input, ArduinoJson::JsonObject &output){
  output["msg"] = VERSION;
}

static void _empty(ArduinoJson::JsonObject &input, ArduinoJson::JsonObject &output){
}

#ifdef ESP8266
static void _updateFirmware(ArduinoJson::JsonObject &inJson, ArduinoJson::JsonObject &outJson){
  MeArm::mainInstance->_updateFWflag = true;
}
static void _updateUI(ArduinoJson::JsonObject &inJson, ArduinoJson::JsonObject &outJson){
  MeArm::mainInstance->_updateUIflag = true;
}
#endif //ESP8266
