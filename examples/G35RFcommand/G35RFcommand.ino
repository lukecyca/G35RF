#include "G35RF.h"

void setup() {
  G35RF_enable();
  Serial.begin(9600);
}

void loop() {
  switch(G35RF_get_command()) {
  case G35RF_POWER_ON:
    Serial.println("ON");
    break;
  case G35RF_POWER_OFF:
    Serial.println("OFF");
    break;
  case G35RF_FUNCTION:
    Serial.println("FUNCTION");
    break;
  }
}
