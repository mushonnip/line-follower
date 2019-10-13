void enableSensor(int L, int R) {
 digitalWrite(pin_ENABLE_SENSORL, L);
 digitalWrite(pin_ENABLE_SENSORR, R);
 delay(1);
}

void init_sensor() {
 int i = 0;
 for (i = 0; i < 14; i++) {
   pinMode(pin_ADC_SENSOR[i], INPUT);
 }
 pinMode(pin_ENABLE_SENSORL, OUTPUT);
 pinMode(pin_ENABLE_SENSORR, OUTPUT);
 enableSensor(0, 0);
}

int readSensor() {
 int bitSensor = 0;
 int i;
 enableSensor(1, 0);
 for (i = 0; i < 7; i++) {
   if (analogRead(pin_ADC_SENSOR[i]) > EE.LIMIT_VALUE_SENSOR[i]) {
     bitSensor = bitSensor | (0b10000000000000 >> i);
   }
 }
 enableSensor(0, 1);
 for (i = 7; i < 14; i++) {
   if (analogRead(pin_ADC_SENSOR[i]) > EE.LIMIT_VALUE_SENSOR[i]) {
     bitSensor = bitSensor | ( 0b10000000000000 >> i);
   }
 }
 enableSensor(0, 0);
 return bitSensor;
}

int calibrate_sensor() {
 int i, valSensor, xCursor = 0;
 int minVal[14], maxVal[14];
 lcd.clear();
 for (i = 0; i < 14; i++) {
   minVal[i] = 1023;
   maxVal[i] = 0;
 }
 while (1) {
 enableSensor(1, 0);
 for (i = 0; i < 7; i++) {
   valSensor = analogRead(pin_ADC_SENSOR[i]);
   if (valSensor > maxVal[i]) {
     maxVal[i] = valSensor;
   }
   if (valSensor < minVal[i]) {
     minVal[i] = valSensor;
   }
 }
 enableSensor(0, 1);
 for (i = 7; i < 14; i++) {
   valSensor = analogRead(pin_ADC_SENSOR[i]);
   if (valSensor > maxVal[i]) {
     maxVal[i] = valSensor;
   }
   if (valSensor < minVal[i]) {
     minVal[i] = valSensor;
   }
 }
 enableSensor(0, 0);
 if (!button_START) {
   break;
 }
 lcd.setCursor(0, 0);
 lcd.print("Scanning Sensor");

 if (millis() % 25 == 0) {
   lcd.setCursor(xCursor, 1);
   lcd.write(0xff);
   if (++xCursor > 15) {
     xCursor = 0;
     lcd.clear();
   }
  }
 }
 for (i = 0; i < 14; i++) {
   EE.LIMIT_VALUE_SENSOR[i] = ((maxVal[i] - minVal[i]) / 2) + minVal[i];
 }
 lcd.clear();
 lcd.setCursor(0, 0);
 lcd.print("Saving...");
 EEPROM.put(0, EE);
 delay(500);
 lcd.clear();
}
