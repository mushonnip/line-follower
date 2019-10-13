#include "hardware_config.h"

#include <LiquidCrystal.h>;
LiquidCrystal lcd(pin_LCDRS, pin_LCDE, pin_LCDD4, pin_LCDD5, pin_LCDD6, pin_LCDD7);

#include <EEPROM.h>
struct EEPROMDATA {
 int SPEED;
 int LIMIT_VALUE_SENSOR[14];
 int MAZE_MODE;
 int INDEXDATA[100];
 char NAME[16];
};

EEPROMDATA EE;

#include "button_and_motor.h"
#include "sensor.h"

#define MAZE_LEFT 0
#define MAZE_RIGHT 1
#define MAZE_RUN 2
#define FWD 0
#define LEFT 1
#define RIGHT 2
#define STOP 3


void setup() {
 // put your setup code here, to run once:
 init_button();
 init_sensor();
 pinMode(pin_LCDRW, OUTPUT);
 pinMode(pin_LCDLED, OUTPUT);
 digitalWrite(pin_LCDRW, LOW);
 digitalWrite(pin_LCDLED, HIGH);
 lcd.begin(16, 2);
 lcd.clear();
 delay(100);
 lcd.setCursor(0, 0);
 lcd.print("hai...");
 lcd.setCursor(0, 1);
 lcd.print("Hello World!!");
 delay(1000);
 lcd.clear();

 EEPROM.get(0, EE);
 if (strcmp(EE.NAME, "linebot") != 0 || !button_DOWNL) {
   lcd.clear();
   lcd.setCursor(0, 0);
   lcd.print("Init EEPROM data");
   memset(EE.LIMIT_VALUE_SENSOR, 0, sizeof(EE.LIMIT_VALUE_SENSOR));
   EE.SPEED = 80;
   EE.MAZE_MODE = MAZE_RUN;
   memset(EE.INDEXDATA, FWD, sizeof(EE.INDEXDATA));
   strcpy(EE.NAME, "linebot");
   EEPROM.put(0, EE);
   lcd.setCursor(0, 1);
   lcd.print("Completed.");
   delay(1000);
   lcd.clear();
 }
}


void loop() {
 // put your main code here, to run repeatedly:
 displayHomeScreen();
 if (!button_START) {
   switch (EE.MAZE_MODE) {
   case MAZE_LEFT:
     run_maze_left();
   break;
   case MAZE_RIGHT:
     run_maze_right();
   break;
   case MAZE_RUN:
     goooooooo();
   break;
   }
 }
 if (!button_MENU) {
   setting();
 }

}

void displayHomeScreen() {
 int i;
 int valSensor = readSensor();
 lcd.setCursor(1, 0);
 for (i = 0; i < 14; i++) {
   if (valSensor & (0b10000000000000 >> i)) {
   lcd.write(0xff);
 } else {
   lcd.write('_');
 }
 }

 lcd.setCursor(11, 1);
 char buff[16];
 sprintf(buff, "V:%3d", EE.SPEED);
 lcd.print(buff);
 if (!button_UPR) {
   if (++EE.SPEED > 255) EE.SPEED = 0;
   EEPROM.put(0, EE);
   delay(200);
 }
 if (!button_DOWNR) {
   if (--EE.SPEED < 0) EE.SPEED = 255;
   EEPROM.put(0, EE);
   delay(200);
 }

 lcd.setCursor(0, 1);
 switch (EE.MAZE_MODE) {
 case MAZE_LEFT:
   lcd.print("MAZE LEFT ");
 break;
 case MAZE_RIGHT:
   lcd.print("MAZE RIGHT");
 break;
 case MAZE_RUN:
   lcd.print("MAZE RUN ");
 break;
 }

 if (!button_UPL) {
   if (++EE.MAZE_MODE > 2) EE.MAZE_MODE = 0;
   EEPROM.put(0, EE);
   delay(200);
 }
 if (!button_DOWNL) {
   if (--EE.MAZE_MODE < 0) EE.MAZE_MODE = 2;
   EEPROM.put(0, EE);
   delay(200);
 }
}

int indexVal = 0;
int stopIndex = 99;
void goooooooo() {
 lcd.clear();
 lcd.print("GOOOO....");
 delay(200);
 digitalWrite(pin_LCDLED, LOW);
 indexVal = 0;
 while (1) {
   followLine();
   if (solve_maze() == 0) {
     break;
   }
   if (!button_START) {
     break;
   }
 }
 setMotor(0, 0);
 digitalWrite(pin_LCDLED, HIGH);
 lcd.clear();
 lcd.print("Completed....");
 delay(2000);
 lcd.clear();
}


int error = 0;
int lastError = 0;
int kp = 15;
int kd = 100;

void followLine() {
 int sensor = readSensor();
 switch (sensor) {
 case 0b00000000000001: error = -13; break;
 case 0b00000000000011: error = -12; break;
 case 0b00000000000010: error = -11; break;
 case 0b00000000000110: error = -10; break;
 case 0b00000000000100: error = -9; break;
 case 0b00000000001100: error = -8; break;
 case 0b00000000001000: error = -7; break;
 case 0b00000000011000: error = -6; break;
 case 0b00000000010000: error = -5; break;
 case 0b00000000110000: error = -4; break;
 case 0b00000000100000: error = -3; break;
 case 0b00000001100000: error = -2; break;
 case 0b00000001000000: error = -1; break;
 case 0b00000011000000: error = 0; break;
 case 0b00000010000000: error = 1; break;
 case 0b00000110000000: error = 2; break;
 case 0b00000100000000: error = 3; break;
 case 0b00001100000000: error = 4; break;
 case 0b00001000000000: error = 5; break;
 case 0b00011000000000: error = 6; break;
 case 0b00010000000000: error = 7; break;
 case 0b00110000000000: error = 8; break;
 case 0b00100000000000: error = 9; break;
 case 0b01100000000000: error = 10; break;
 case 0b01000000000000: error = 11; break;
 case 0b11000000000000: error = 12; break;
 case 0b10000000000000: error = 13; break;
 }

 int rateError = error - lastError;
 lastError = error;
 int moveVal = (int) (error * kp) + (rateError * kd);
 int moveLeft = EE.SPEED - moveVal;
 int moveRight = EE.SPEED + moveVal;
 int minSpeed = -125;
 int maxSpeed = 255;
 moveLeft = constrain(moveLeft, minSpeed, maxSpeed);
 moveRight = constrain(moveRight, minSpeed, maxSpeed);
 setMotor(moveLeft, moveRight);
}

int solve_maze() {
 int sensor = readSensor();
 int sensorMid = sensor & 0b00000111100000;
 int sensorLeft = sensor & 0b00000000000111;
 int sensorRight = sensor & 0b11100000000000;
 int get_index = 0;
 if (sensorMid && sensorLeft ) {
   get_index = 1;
 }else if (sensorMid && sensorRight ) {
   get_index = 1;
 }else if (sensorRight && sensorLeft ) {
   get_index = 1;
 };
 if (get_index == 1) {
   digitalWrite(pin_LCDLED, HIGH);
   indexVal ++;
   if ( indexVal > 99 ) {
     return 0;
   }
   if (EE.INDEXDATA[indexVal] == STOP) {
     return 0;
   }else if (EE.INDEXDATA[indexVal] == LEFT) {
      setMotor(-125, 200);
      delay(100);
   } else if (EE.INDEXDATA[indexVal] == RIGHT) {
      setMotor(200, -125);
      delay(100);
   } else if (EE.INDEXDATA[indexVal] == FWD) {
   setMotor(EE.SPEED, EE.SPEED);
   unsigned long startMillis = millis();
   unsigned long interVal = 150;
   while (millis() - startMillis < interVal) {
     followLine();
   }
 }
 lcd.setCursor(0, 1);
 lcd.print("LastIndex:");
 lcd.print(indexVal);
 lcd.print(" ");
 if (EE.INDEXDATA[indexVal] == LEFT) {
   lcd.print("LEFT");
 } else if (EE.INDEXDATA[indexVal] == RIGHT) {
   lcd.print("RGHT");
 } else if (EE.INDEXDATA[indexVal] == FWD) {
   lcd.print("FWD ");
 }
   digitalWrite(pin_LCDLED, LOW);
 }
 return 1;
}

void setting() {
 lcd.clear();
 delay(200);
 int menu = 0;
 while (1) {
   if (!button_UPL) {
     if (++menu > 1) menu = 0;
     delay(200);
   }
   if (!button_DOWNL) {
     if (--menu < 0) menu = 1;
     delay(200);
   }
   switch (menu) {
   case 0:
     lcd.setCursor(0, 0);
     lcd.print("> View DataIndex");
     lcd.setCursor(0, 1);
     lcd.print(" Scan Sensor ");
     if (!button_MENU)viewDataIndex() ;
   break;
   case 1:
     lcd.setCursor(0, 0);
     lcd.print(" View DataIndex");
     lcd.setCursor(0, 1);
     lcd.print("> Scan Sensor ");
   if (!button_MENU)calibrate_sensor() ;
   break;
   }
   if (!button_START) {
     break;
   }
 }
 lcd.clear();
 delay(200);
}

void viewDataIndex() {
 lcd.clear();
 delay(200);
 int i = 1;
 while (1) {
   lcd.setCursor(0, 0);
   lcd.print("index: ");
   lcd.print(i);
   lcd.print(" ");
   if (!button_UPL) {
     if (++i > 99) i = 1;
     delay(200);
   }
   if (!button_DOWNL) {
     if (--i < 1) i = 99;
     delay(200);
   }

   lcd.setCursor(0, 1);
   if (EE.INDEXDATA[i] == LEFT) {
     lcd.print("Value: LEFT ");
   } else if (EE.INDEXDATA[i] == RIGHT) {
     lcd.print("Value: RIGHT ");
   } else if (EE.INDEXDATA[i] == FWD) {
     lcd.print("Value: FWD ");
   } else if (EE.INDEXDATA[i] == STOP) {
     lcd.print("Value: STOP ");
   } else {
     lcd.print("Value: ERROR");
   }
   if (!button_UPR) {
   if (++EE.INDEXDATA[i] > STOP) EE.INDEXDATA[i] = FWD;
     EEPROM.put(0, EE);
    delay(200);
   }
   if (!button_DOWNR ) {
   if (--EE.INDEXDATA[i] < FWD) EE.INDEXDATA[i] = STOP;
    EEPROM.put(0, EE);
    delay(200);
   }
   if (!button_START) {
     break;
   }
 }

 lcd.clear();
 delay(200);
}

void run_maze_left() {
 lcd.clear();
 delay(200);
 lcd.setCursor(0, 0);
 lcd.print("Kamu Bisa Bikin");
 lcd.setCursor(0, 1);
 lcd.print("programnya?");
 while (button_START);
 delay(200);
 lcd.clear();
}

void run_maze_right() {
 lcd.clear();
 delay(200);
 lcd.setCursor(0, 0);
 lcd.print("Kamu Bisa Bikin");
 lcd.setCursor(0, 1);
 lcd.print("programnya?");
 while (button_START);
 delay(200);
 lcd.clear();
}
