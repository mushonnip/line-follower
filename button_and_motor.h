#define button_UPL digitalRead(pin_button_UPL)
#define button_DOWNL digitalRead(pin_button_DOWNL)
#define button_UPR digitalRead(pin_button_UPR)
#define button_DOWNR digitalRead(pin_button_DOWNR)
#define button_START digitalRead(pin_button_START)
#define button_MENU digitalRead(pin_button_MENU)

void init_button() {
 pinMode(pin_button_UPL, INPUT_PULLUP);
 pinMode(pin_button_DOWNL, INPUT_PULLUP);
 pinMode(pin_button_UPR, INPUT_PULLUP);
 pinMode(pin_button_DOWNR, INPUT_PULLUP);
 pinMode(pin_button_START, INPUT_PULLUP);
 pinMode(pin_button_MENU, INPUT_PULLUP);
}

void setMotor(int L, int R) {
 if (L > 0) {
   digitalWrite(pin_MOTOR_DIRL, LOW);
 } else {
   digitalWrite(pin_MOTOR_DIRL, HIGH);
   L = 255+L;
 }
 analogWrite(pin_MOTOR_PWML, L);
 if (R > 0) {
   digitalWrite(pin_MOTOR_DIRR, LOW);
 } else {
   digitalWrite(pin_MOTOR_DIRR, HIGH);
   R = 255+R;
 }
 analogWrite(pin_MOTOR_PWMR, R);
