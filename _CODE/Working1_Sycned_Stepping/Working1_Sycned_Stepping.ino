//MACRO//
#define X_STEP_PIN 54
#define X_DIR_PIN  55
#define X_ENABLE_PIN 38
#define X_STEP_HIGH PORTF  |= B00000001;
#define X_STEP_LOW PORTF  &= ~B00000001;

#define Y_STEP_PIN 60
#define Y_DIR_PIN  61
#define Y_ENABLE_PIN 56
#define Y_STEP_HIGH   PORTF  |= B01000000;
#define Y_STEP_LOW    PORTF  &= ~B01000000;

#define A_STEP_PIN 26
#define A_DIR_PIN  28
#define A_ENABLE_PIN 24
#define A_STEP_HIGH PORTA  |= B00010000;
#define A_STEP_LOW PORTA  &= ~B00010000;

#define TIMER1_ON TIMSK1 |=(1<<OCIE1A);
#define TIMER1_OFF TIMSK1 &=~(1<<OCIE1A);


//STRUCT INFO//
struct joint {
  unsigned int startSpeed;
  volatile unsigned long maxSpeed;
  volatile unsigned long rampIndex;
  volatile unsigned long stepCount;
  volatile unsigned long totalSteps;
  volatile unsigned long accelStepCount;
  volatile float dMicros;
  volatile unsigned long dTrunc;
  volatile bool moveDone;

  volatile float timeScalar;
  unsigned long totalMoveDuration;
  unsigned long accelStepCountEst;
  unsigned long accelDuration;

  void (*setDirection) (int);
  void (*step) ();

  long position;
  volatile int direction;
  unsigned long stepsPerRevolution;
};


//STRUCT FUNCTIONS//
void xStep () {
  X_STEP_HIGH
  X_STEP_LOW
}
void xDirection (int dir) {
  digitalWrite (X_DIR_PIN, dir > 0 ? HIGH : LOW);
}
void yStep () {
  Y_STEP_HIGH
  Y_STEP_LOW
}
void yDirection (int dir) {
  digitalWrite (Y_DIR_PIN, dir > 0 ? HIGH : LOW);
}
void aStep () {
  A_STEP_HIGH
  A_STEP_LOW
}
void aDirection (int dir) {
  digitalWrite (A_DIR_PIN, dir > 0 ? HIGH : LOW);
}


//GLOBALS//
#define NUM_JOINTS 3
joint jointArray[NUM_JOINTS];
volatile byte jointRemain = 0;
volatile byte nextJoint = 0;
volatile float accelScalar = 0;
int resetButtonPin = 47;

//SETUP//
void setup() {
  Serial.begin(115200);
  pinMode (X_STEP_PIN, OUTPUT);
  pinMode (X_DIR_PIN, OUTPUT);
  pinMode (X_ENABLE_PIN, OUTPUT);
  digitalWrite (X_ENABLE_PIN, LOW);

  pinMode (Y_STEP_PIN, OUTPUT);
  pinMode (Y_DIR_PIN, OUTPUT);
  pinMode (Y_ENABLE_PIN, OUTPUT);
  digitalWrite (Y_ENABLE_PIN, LOW);

  pinMode (A_STEP_PIN, OUTPUT);
  pinMode (A_DIR_PIN, OUTPUT);
  pinMode (A_ENABLE_PIN, OUTPUT);
  digitalWrite (A_ENABLE_PIN, LOW);

  noInterrupts();
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
  OCR1A = 1000;
  TCCR1B |= (1 << WGM12);
  TCCR1B |= (0 << CS12) | (1 << CS11) | (1 << CS10);
  interrupts();

  jointArray[0].startSpeed = 1700;
  jointArray[0].maxSpeed = 85; //215
  jointArray[0].setDirection = xDirection;
  jointArray[0].step = xStep;

  jointArray[1].startSpeed = 2500;
  jointArray[1].maxSpeed = 255;
  jointArray[1].setDirection = yDirection;
  jointArray[1].step = yStep;

  jointArray[2].startSpeed = 1300;
  jointArray[2].maxSpeed = 80;
  jointArray[2].setDirection = aDirection;
  jointArray[2].step = aStep;

  //gear ratio setup
  double J4_gear_ratio = (37.0 / 23.0);
  long J4_one_rev_num_steps = ( 200 * 4 * J4_gear_ratio * 19);
  jointArray[2].stepsPerRevolution = J4_one_rev_num_steps;

  float J5_gear_ratio = (37.0 / 23.0); //1.6086
  float J5_gear_ratio_2 = (55.0 / 77.0); //0.7142
  long J5_one_rev_num_steps = ( 200 * 4 * J5_gear_ratio * J5_gear_ratio_2 * 11); //10109.98
  J5_one_rev_num_steps = 10800.0;

  float yGearRatio = (55.0 / 69.0); //0.7971
  long yOneRevolution = (200 * 4 * yGearRatio * 14); /// 8927
  yOneRevolution = 7175;
  jointArray[0].stepsPerRevolution = J5_one_rev_num_steps;
  jointArray[1].stepsPerRevolution = yOneRevolution;

}


//ISR//
ISR(TIMER1_COMPA_vect) {
  unsigned int previousTimeSlice = OCR1A;
  OCR1A = 65500;

  for (int i = 0; i < NUM_JOINTS; i++) {
    if ( ! ((1 << i) & jointRemain) )
      continue;
    if ( ! ((1 << i) & nextJoint) ) {
      jointArray[i].dTrunc -= previousTimeSlice;
      continue;
    }

    joint &ji = jointArray[i];
    if (ji.stepCount < ji.totalSteps) {
      ji.step();
      ji.stepCount++;
      ji.position += ji.direction;
      if (ji.stepCount >= ji.totalSteps) {
        ji.moveDone = true;
        jointRemain &= ~(1 << i);
      }
      if (ji.accelStepCount == 0) {
        ji.rampIndex++;
        ji.dMicros = ji.dMicros - (2 * ji.dMicros) / (4 * ji.rampIndex + 1 ); ///flash
        if ( ji.dMicros <= ji.maxSpeed) {
          ji.dMicros = ji.maxSpeed;
          ji.accelStepCount = ji.stepCount;
        }
        if ( ji.stepCount >= ji.totalSteps / 2 ) {
          ji.accelStepCount = ji.stepCount;
        }
      } else if (ji.stepCount >= ji.totalSteps - ji.accelStepCount) {
        //ji.dMicros = ji.dMicros + (2 * ji.dMicros) / (4 * ji.rampIndex + 1 - 2);//flash
        ji.dMicros = ( ji.dMicros * (4 * ji.rampIndex + 1) ) / (4 * ji.rampIndex + 1 - 2  );
        ji.rampIndex--;
      }
      ji.dTrunc = ji.dMicros * ji.timeScalar;
    }
  }
  calcNextInterval();
  TCNT1  = 0;
}


//NORMALIZE ACCELERATIONS
void normalizeAccelerations() {
  unsigned long largestMoveDuration = 0 ;

  for (int i = 0; i < NUM_JOINTS; i++) {
    if ( (jointArray[i].totalMoveDuration > largestMoveDuration) && ((1 << i) & jointRemain) ) {
      largestMoveDuration = jointArray[i].totalMoveDuration;
    }
  }
  for (int i = 0; i < NUM_JOINTS; i++) {
    if ( ((1 << i) &  jointRemain) ) {
      jointArray[i].timeScalar = float(largestMoveDuration) / float(jointArray[i].totalMoveDuration);
    }
  }
}


//GET DURATION OF ACCELERATION
float getDurationOfAcceleration (unsigned long accelStepCountEst, unsigned long startSpeed) {
  float accelDuration = 0;
  float timeSlice = startSpeed;

  for (unsigned int i = 1; i < accelStepCountEst; i++) {
    timeSlice = timeSlice - (2 * timeSlice) / (4 * i + 1 );
    accelDuration += timeSlice;
  }
  return accelDuration;
}


//ESTIMATE ACCEL STEP COUNT
void estimateAccelStepCount (joint& ji) {
  float timeSlice = ji.startSpeed;

  for (int i = 1; i < ji.totalSteps / 2; i++) {
    timeSlice = timeSlice - (2 * timeSlice) / (4 * i + 1  );
    //Serial.println(timeSlice);
    ji.accelStepCountEst++;
    if (timeSlice < ji.maxSpeed) {
      return;
    }
  }
}


//PREPARE MOVE//
void prepareMove(int whichJoint, long desiredPosition) {
  joint &ji = jointArray[whichJoint]; // type
  long numSteps = desiredPosition - ji.position;
  ji.stepCount = 0;
  ji.accelStepCount = 0;
  ji.rampIndex = 0;
  ji.dMicros = ji.startSpeed;
  ji.dTrunc = ji.dMicros;
  ji.totalSteps = abs (numSteps);
  ji.moveDone = false;
  if (ji.totalSteps != 0) {
    jointRemain |= (1 << whichJoint);
  }
  ji.timeScalar = 1.0;
  ji.accelDuration = 0;
  ji.totalMoveDuration = 0;
  ji.accelStepCountEst = 0;

  ji.setDirection  (numSteps > 0 ? 1 : -1);
  ji.direction = (numSteps > 0 ? 1 : -1);

  estimateAccelStepCount (ji);

  float tempStartSpeed = ji.startSpeed;

  //will max speed before halfway
  if ( ji.accelStepCountEst >= ji.totalSteps / 2) {
    ji.accelStepCountEst = ji.totalSteps / 2;
    ji.accelDuration = getDurationOfAcceleration (ji.accelStepCountEst, tempStartSpeed);
    ji.totalMoveDuration = ji.accelDuration * 2;
  } else { //some time spent at max speed
    ji.accelDuration = getDurationOfAcceleration (ji.accelStepCountEst, tempStartSpeed);
    ji.totalMoveDuration = (ji.accelDuration * 2) + ( ji.maxSpeed * (ji.totalSteps - (2 * ji.accelStepCountEst)) );
  }
}


//CALCULATE NEXTINTERVAL//
void calcNextInterval () {
  volatile unsigned int previousTimeSlice = 9999999;

  for (int i = 0; i < NUM_JOINTS; i++) {
    if ( (jointArray[i].dTrunc < previousTimeSlice) && ((1 << i) & jointRemain) ) {
      previousTimeSlice = jointArray[i].dTrunc;
    }
  }
  nextJoint = 0;
  for (int i = 0; i < NUM_JOINTS; i++) {
    if ( (jointArray[i].dTrunc == previousTimeSlice) && ((1 << i) & jointRemain) ) {
      nextJoint |= (1 << i);
    }
  }
  if (nextJoint == 0) {
    OCR1A = 65500;
    TIMER1_OFF
  }
  OCR1A = previousTimeSlice;
}


//RUN AND WAIT//
void runAndWait() {
  normalizeAccelerations();
  calcNextInterval();
  TIMER1_ON
  while (jointRemain != 0);
  nextJoint = 0;
  jointRemain = 0;
}


//RESET HOME POSITION//
void resetHomePosition () {
  for (int i = 0; i < NUM_JOINTS; i++) {
    jointArray[i].position = 0;
  }
  delay(500);
}

//DoubleSpeed
void doubleSpeed () {
  TCCR1B &= ~(1 << CS10);
  jointArray[0].maxSpeed *= 2;
  jointArray[1].maxSpeed *= 2;
  jointArray[2].maxSpeed *= 2;
  jointArray[0].startSpeed *= 2;
  jointArray[1].startSpeed *= 2;
  jointArray[2].startSpeed *= 2;

}

//Serial Input
void serialInput() {


  //SERIAL INPUT//
  unsigned long xInput, yInput, aInput;
  char rawData[100] = "";   //start with clear array
  Serial.println("Input position seperated by spaces");

  while (1) {
    //button for reset
    if ( digitalRead(resetButtonPin) == HIGH ) {
      resetHomePosition();
    }
    //reads in data from serial command line
    if (Serial.available() > 0) { //new Data in
      size_t byteCount = Serial.readBytesUntil('\n', rawData, sizeof(rawData) - 1);  // set serial monitor to "newline"
      rawData[byteCount] = NULL;  //put the end char on the string
      Serial.print("Raw Data = ");
      Serial.println(rawData);

      ////////////parsing our data
      const char delimiter[] = " ";
      char parsedStrings [5][20]; //array of strings, maxtrix of char
      char *token = strtok(rawData, delimiter);
      // check if NULL
      strncpy(parsedStrings[0], token, sizeof(parsedStrings[0]));
      for (int i = 1; i < 5; i++)
      {
        token = strtok(NULL, delimiter);//goes to the next
        strncpy(parsedStrings[i], token, sizeof(parsedStrings[i]));
      }
      for (int i = 0; i < 5; i++)
        Serial.println( parsedStrings[i]);

      ////////////////converting our data
      xInput = atol(parsedStrings[0]);
      Serial.println(xInput);
      yInput = atol(parsedStrings[1]);
      Serial.println(yInput);
      aInput = atol(parsedStrings[2]);
      Serial.println(aInput);
      if ( jointArray[0].position - xInput != 0 ) {
        prepareMove(0, xInput);
      }
      if ( jointArray[1].position - yInput != 0 ) {
        prepareMove(1, yInput);
      }
      if ( jointArray[2].position - aInput != 0 ) {
        prepareMove(2, aInput);
      }
      runAndWait();
    }
  }//end while

}

///////////////////////////////////////LOOP//
void loop() {
  delay(1000);

  resetHomePosition();
  doubleSpeed();
  showRoutine1();
  serialInput();

  delay(500);

  while ( true );

}
