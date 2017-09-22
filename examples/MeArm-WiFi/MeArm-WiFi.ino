#include <MeArm.h>
#include "web.h"

MeArm meArm;

void setup(){
  meArm.begin();
}

void loop(){
  meArm.loop();
}