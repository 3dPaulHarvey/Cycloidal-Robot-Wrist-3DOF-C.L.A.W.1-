int num_buttons_pressed = 0;

//j4
#define X_DIR_PIN          55
#define X_STEP_PIN         54
#define X_ENABLE_PIN       38
float J5_gear_ratio = (37.0 / 23.0);
float J5_gear_ratio_2 = (55.0 / 77.0);
long J5_one_rev_num_steps = ( 200 * 4 * J5_gear_ratio * J5_gear_ratio_2 * 11);
float J5_J6_axis_gear_ratio = (55.0 / 69.0);
int J4_CW_button_pin = 16;
int J4_CCW_button_pin = 17;



//j5
#define Y_DIR_PIN          61
#define Y_STEP_PIN         60
#define Y_ENABLE_PIN       56
double J4_gear_ratio = (37.0 / 23.0);
long J4_one_rev_num_steps = ( 200 * 4 * J4_gear_ratio * 19);
int J5_CW_button_pin = 25;
int J5_CCW_button_pin = 23;


//j6
#define A_DIR_PIN          28
#define A_STEP_PIN         26
#define A_ENABLE_PIN       24
int J6_CW_button_pin = 47;
int J6_CCW_button_pin = 32;




void setup() {
  Serial.begin(115200);
  // Declare pins as output:
  pinMode(X_STEP_PIN, OUTPUT);
  pinMode(X_DIR_PIN, OUTPUT);
  pinMode(X_ENABLE_PIN, OUTPUT);
  digitalWrite(X_ENABLE_PIN, LOW);
  digitalWrite(X_DIR_PIN, HIGH);

  pinMode(Y_STEP_PIN, OUTPUT);
  pinMode(Y_DIR_PIN, OUTPUT);
  pinMode(Y_ENABLE_PIN, OUTPUT);
  digitalWrite(Y_ENABLE_PIN, LOW);

  pinMode(A_STEP_PIN, OUTPUT);
  pinMode(A_DIR_PIN, OUTPUT);
  pinMode(A_ENABLE_PIN, OUTPUT);
  digitalWrite(A_ENABLE_PIN, LOW);

}

void loop() {
  int J4_CW_button = digitalRead(J4_CW_button_pin);
  int J4_CCW_button = digitalRead(J4_CCW_button_pin);
  int J5_CW_button = digitalRead(J5_CW_button_pin);
  int J5_CCW_button = digitalRead(J5_CCW_button_pin);
  int J6_CW_button = digitalRead(J6_CW_button_pin);
  int J6_CCW_button = digitalRead(J6_CCW_button_pin);

  num_buttons_pressed = J4_CW_button + J5_CW_button;


  if (digitalRead(J4_CW_button_pin) == HIGH) {
    digitalWrite(X_DIR_PIN, LOW);
    digitalWrite(X_STEP_PIN, HIGH);
    delayMicroseconds(200);
    digitalWrite(X_STEP_PIN, LOW);
    delayMicroseconds(200);

  } else  if (digitalRead(J4_CCW_button_pin) == HIGH) {
    digitalWrite(X_DIR_PIN, HIGH);
    digitalWrite(X_STEP_PIN, HIGH);
    delayMicroseconds(200);
    digitalWrite(X_STEP_PIN, LOW);
    delayMicroseconds(200);

  }

  if (digitalRead(J5_CW_button_pin) == HIGH) {
    digitalWrite(Y_DIR_PIN, LOW);
    digitalWrite(Y_STEP_PIN, HIGH);
    delayMicroseconds(200);
    digitalWrite(Y_STEP_PIN, LOW);
    delayMicroseconds(200);

  }
  if (digitalRead(J5_CCW_button_pin) == HIGH) {
    digitalWrite(Y_DIR_PIN, HIGH);
    digitalWrite(Y_STEP_PIN, HIGH);
    delayMicroseconds(200);
    digitalWrite(Y_STEP_PIN, LOW);
    delayMicroseconds(200);

  }

  
  if (digitalRead(J6_CW_button_pin) == HIGH) {

    digitalWrite(A_DIR_PIN, LOW);
    digitalWrite(A_STEP_PIN, HIGH);
    delayMicroseconds(150);
    digitalWrite(A_STEP_PIN, LOW);
    delayMicroseconds(150);

  }
  if (digitalRead(J6_CCW_button_pin) == HIGH) {
 
    digitalWrite(A_DIR_PIN, HIGH);
    digitalWrite(A_STEP_PIN, HIGH);
    delayMicroseconds(150);
    digitalWrite(A_STEP_PIN, LOW);
    delayMicroseconds(150);

  }


  /*
    //////j4//////////
    digitalWrite(X_DIR_PIN, HIGH);

    delay(2000);

    // Spin the stepper motor 1 revolution slowly:
    for (int i = 0; i < J4_one_rev_num_steps; i++) {
      // These four lines result in 1 step:
      digitalWrite(X_STEP_PIN, HIGH);
      delayMicroseconds(100);
      digitalWrite(X_STEP_PIN, LOW);
      delayMicroseconds(100);
    }

    delay(1000);

    // Set the spinning direction counterclockwise:
    digitalWrite(X_DIR_PIN, LOW);

    // Spin the stepper motor 1 revolution quickly:
    for (int i = 0; i < J4_one_rev_num_steps; i++) {
      // These four lines result in 1 step:
      digitalWrite(X_STEP_PIN, HIGH);
      delayMicroseconds(100);
      digitalWrite(X_STEP_PIN, LOW);
      delayMicroseconds(100);
    }


    //////j5////////////
    digitalWrite(Y_DIR_PIN, HIGH);

    delay(2000);

    // Spin the stepper motor 1 revolution slowly:
    for (int i = 0; i < J5_one_rev_num_steps; i++) {
    // These four lines result in 1 step:
    digitalWrite(Y_STEP_PIN, HIGH);
    delayMicroseconds(100);
    digitalWrite(Y_STEP_PIN, LOW);
    delayMicroseconds(100);
    }

    delay(1000);

    // Set the spinning direction counterclockwise:
    digitalWrite(Y_DIR_PIN, LOW);

    // Spin the stepper motor 1 revolution quickly:
    for (int i = 0; i < J5_one_rev_num_steps; i++) {
    // These four lines result in 1 step:
    digitalWrite(Y_STEP_PIN, HIGH);
    delayMicroseconds(100);
    digitalWrite(Y_STEP_PIN, LOW);
    delayMicroseconds(100);
    }



    //////j6//////////
    digitalWrite(A_DIR_PIN, HIGH);

    delay(2000);

    // Spin the stepper motor 1 revolution slowly:
    for (int i = 0; i < J5_one_rev_num_steps; i++) {
    // These four lines result in 1 step:
    digitalWrite(A_STEP_PIN, HIGH);
    delayMicroseconds(100);
    digitalWrite(A_STEP_PIN, LOW);
    delayMicroseconds(100);
    }

    delay(1000);

    // Set the spinning direction counterclockwise:
    digitalWrite(A_DIR_PIN, LOW);

    // Spin the stepper motor 1 revolution quickly:
    for (int i = 0; i < J5_one_rev_num_steps; i++) {
    // These four lines result in 1 step:
    digitalWrite(A_STEP_PIN, HIGH);
    delayMicroseconds(100);
    digitalWrite(A_STEP_PIN, LOW);
    delayMicroseconds(100);
    }

  */

}
