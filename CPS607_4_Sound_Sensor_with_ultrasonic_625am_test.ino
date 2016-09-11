#include <NewPing.h>
#include <A4990MotorShield.h>
#include <LiquidCrystal.h>
#include <QTRSensors.h>

A4990MotorShield motors;

#define NUM_SENSORS             3  // number of sensors used
#define NUM_SAMPLES_PER_SENSOR  4  // average 4 analog samples per sensor reading
#define EMITTER_PIN             QTR_NO_EMITTER_PIN  // no emitter pin


//motor + ultrasonic
#define TRIGGER_PIN_LEFT          33
#define ECHO_PIN_LEFT             32
#define TRIGGER_PIN_CENTER_LEFT   37
#define ECHO_PIN_CENTER_LEFT      36
#define TRIGGER_PIN_CENTER_RIGHT  45
#define ECHO_PIN_CENTER_RIGHT     44
#define TRIGGER_PIN_RIGHT         41
#define ECHO_PIN_RIGHT            40
#define MAX_DISTANCE              500
#define MIN_DISTANCE              20
#define MOVE_TIME                 150
#define TURN_TIME                 400
#define ROBOT_SPEED               105
#define ROBOT_ADJUSTMENT          0

//define the mic analog pins
#define mic1 A0
#define mic2 A2
#define mic3 A3
#define mic4 A4

NewPing sonar_LEFT(TRIGGER_PIN_LEFT, ECHO_PIN_LEFT, MAX_DISTANCE);
NewPing sonar_CENTER_LEFT(TRIGGER_PIN_CENTER_LEFT, ECHO_PIN_CENTER_LEFT, MAX_DISTANCE);
NewPing sonar_CENTER_RIGHT(TRIGGER_PIN_CENTER_RIGHT, ECHO_PIN_CENTER_RIGHT, MAX_DISTANCE);
NewPing sonar_RIGHT(TRIGGER_PIN_RIGHT, ECHO_PIN_RIGHT, MAX_DISTANCE);

QTRSensorsAnalog qtra((unsigned char[]) {10, 14, 15}, 
  NUM_SENSORS, NUM_SAMPLES_PER_SENSOR, EMITTER_PIN);
unsigned int IRsensorValues[NUM_SENSORS];

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(53, 51, 49, 47, 52, 50);

int sensorvalue1=0;
int sensorvalue2=0;
int sensorvalue3=0;
int sensorvalue4=0;

int calibration1=0;
int calibration2=0;
int calibration3=0;
int calibration4=0;

int foundnoise=0;

int threshold=10;

void setup() {
  // set up the LCD's number of columns and rows: 
Serial.begin(9600);
motors.flipM1(true);
motors.flipM2(true);
Serial.println("Initialized");
initial();

go_forward();
ir_delay();
take_avg();
calibration1=sensorvalue1;
calibration2=sensorvalue2;
calibration3=sensorvalue3;
calibration4=sensorvalue4+10;
}

void loop() {

delay(10);


// OBSTACLE AVOIDANCE ---------------------------------------------------------------------------

  int distance_LEFT = sonar_LEFT.ping_cm();
  int distance_CENTER_LEFT = sonar_CENTER_LEFT.ping_cm();
  int distance_CENTER_RIGHT = sonar_CENTER_RIGHT.ping_cm();
  int distance_RIGHT = sonar_RIGHT.ping_cm();
  
    if (distance_LEFT < 10)
   {
    calibration ();    
    turn_right();
   }
  else if (distance_RIGHT < 10)
  {
    calibration ();
    turn_left();
  }
  else if (distance_CENTER_LEFT < 20 || distance_CENTER_RIGHT < 20)
  {
    go_backward();
    if (distance_CENTER_LEFT < 20)
      turn_right();
      else
         turn_left();
  }
  // there is an obstacle in front
  else if (distance_CENTER_LEFT < MIN_DISTANCE && distance_CENTER_RIGHT < MIN_DISTANCE)
  {
    if (distance_LEFT < MIN_DISTANCE)
    {
      calibration ();
      turn_right();
    }
    else if (distance_RIGHT < MIN_DISTANCE)
    {
      calibration ();
      turn_left();
    }
    else
    {
      calibration ();
      go_backward();
      turn_right();
    }
  }
  // there is an obstacle in front left
  else if (distance_CENTER_LEFT < MIN_DISTANCE)
  {
    calibration ();
    turn_right();
  }
  // there is an obstacle in front right
  else if (distance_CENTER_RIGHT < MIN_DISTANCE)
  {
    calibration ();
    turn_left();
  }
  // there is no obstacle in front
  else
  {
    go_forward_sound_check();
  }
   go_forward_sound_check();
}//end loop

void found()
{
  lcd.noAutoscroll();
  lcd.clear();
  lcd.print("Hey YOU! Pls..");
  lcd.setCursor(0,1);
  lcd.print("Quiet Down!");
  delay(2000);
}

void initial()
{
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.setCursor(16, 0);
  lcd.autoscroll();
  {
    lcd.print("L");
    delay(100);
    lcd.print("i");
    delay(100);
    lcd.print("b");
    delay(100);
    lcd.print("r");
   
    lcd.print("a");
    delay(100);
    lcd.print("r");
    delay(100);
    lcd.print("y");
    delay(100);
    lcd.print(" ");
   
    lcd.print("W");
    delay(100);
    lcd.print("a");
    delay(100);
    lcd.print("t");
    delay(100);
    lcd.print("c");
    delay(100);
    lcd.print("h");
    delay(100);
    lcd.print("d");
    delay(100);
    lcd.print("o");
    delay(100);
    lcd.print("g");
    delay(100);
  }
  lcd.noAutoscroll();
  
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Library Watchdog");
    lcd.setCursor(0,1);
    lcd.print("I");
    delay(100);
    lcd.print("n");
    delay(100);
    lcd.print("i");
    delay(100);
    lcd.print("t");
    delay(100);    
    lcd.print("i");
    delay(100);
    lcd.print("a");
    delay(100);
    lcd.print("l");
    delay(100);
    lcd.print("i");
    delay(100);
    lcd.print("z");
    delay(100);
    lcd.print("i");
    delay(100);
    lcd.print("n");
    delay(100);
    lcd.print("g");
    delay(100);
    lcd.print("!");
    delay(1000);
  }
}

void searching()
{
lcd.clear();
  // Print a message to the LCD.

  {
    lcd.setCursor(0, 0);
    lcd.print("db1:");
    lcd.print(sensorvalue1);
    lcd.print(",db2:");
    lcd.print(sensorvalue2);
    lcd.setCursor(0,1);
    lcd.print(",db3:");
    lcd.print(sensorvalue3);
    lcd.print(",db4:");
    lcd.print(sensorvalue4);
  }  
}

int take_avg()
{
  int sample1;
  int sample2;
  int sample3;
  int sample4;
  int total1=0;
  int total2=0;
  int total3=0;
  int total4=0;

  for (int j = 0; j < 50; j++)
  {
    sample1 = analogRead(mic1);
    sample2 = analogRead(mic2);
    sample3 = analogRead(mic3);
    sample4 = analogRead(mic4);
    total1 = total1 + sample1;
    total2 = total2 + sample2;
    total3 = total3 + sample3;
    total4 = total4 + sample4;
  }
  sensorvalue1 = ((int) (total1 / 50)) - calibration1;
  sensorvalue2 = ((int) (total2 / 50)) - calibration2;
  sensorvalue3 = ((int) (total3 / 50)) - calibration3;
  sensorvalue4 = ((int) (total4 / 50)) - calibration4;
}

void go_forward_sound_check()
{
     go_forward();
  // SOUND CHECKING ---------------------------------------------------------------------------
take_avg();
    if(sensorvalue2>100)
    {
      foundnoise=1;
    }
Serial.println("--------------------------");
Serial.print(" || Sensor 1: ");
Serial.print(sensorvalue1);
Serial.print(" || Sensor 2: ");
Serial.print(sensorvalue2);
Serial.print(" || Sensor 3: ");
Serial.print(sensorvalue3);
Serial.print(" || Sensor 4: ");
Serial.println(sensorvalue4);

if(foundnoise==1)//if noise found
{
  while(1)
  {
  stop_turn();
  found();
  delay(500);
  }
}
else //still searching for sound
{
  searching();
}

if (sensorvalue1 >threshold || sensorvalue2 >threshold || sensorvalue3 >threshold || sensorvalue4 >threshold)
{
  if(sensorvalue4>sensorvalue1 && sensorvalue4>sensorvalue2 && sensorvalue4>sensorvalue3)
  {
    half_turn();
  }
  
  else if(sensorvalue3>sensorvalue1 && sensorvalue3>sensorvalue2)
  {
    turn_right();
    ir_delay();
    ir_delay();
    
  }
  
  else if(sensorvalue1>sensorvalue3 && sensorvalue1>sensorvalue2)
  {
    turn_left();
    ir_delay();
    ir_delay();
  }
  
 /* else if((sensorvalue2+3)>=sensorvalue3 && (sensorvalue2+3)>=sensorvalue1 && (sensorvalue2+3)>=sensorvalue4)
  {
    go_forward_avoid();
  }
  */
   else 
   go_forward();
}
}
void go_forward (void)
{
  motors.setM1Speed(ROBOT_SPEED + ROBOT_ADJUSTMENT);
    motors.setM2Speed(ROBOT_SPEED);
ir_delay();
ir_delay();
    // motors.setM1Speed(0);
    // motors.setM2Speed(0);
}

void go_backward (void)
{
  motors.setM1Speed(-(ROBOT_SPEED + ROBOT_ADJUSTMENT));
    motors.setM2Speed(-ROBOT_SPEED);
ir_delay();
ir_delay();
     // motors.setM1Speed(0);
    // motors.setM2Speed(0);
}

void turn_left (void)
{
  motors.setM1Speed(ROBOT_SPEED + ROBOT_ADJUSTMENT);
    motors.setM2Speed(-ROBOT_SPEED);
    ir_delay();
    ir_delay();
  // motors.setM1Speed(0);
    // motors.setM2Speed(0);
}

void turn_right (void)
{
  motors.setM1Speed(-(ROBOT_SPEED + ROBOT_ADJUSTMENT));
    motors.setM2Speed(ROBOT_SPEED);
    ir_delay();
    ir_delay();
    // motors.setM1Speed(0);
    // motors.setM2Speed(0);
}
void stop_turn (void)
{
     motors.setM1Speed(0);
     motors.setM2Speed(0);
}

void half_turn(void)
{
  motors.setM1Speed(-(ROBOT_SPEED + ROBOT_ADJUSTMENT));
    motors.setM2Speed(ROBOT_SPEED);
for(int z=0; z>19; z++)
ir_delay();
 
    motors.setM1Speed(0);
     motors.setM2Speed(0);
}

void calibration (void)
{
  calibration1=0;
  calibration2=0;
  calibration3=0;
  calibration4=0;
  take_avg();
  calibration1=sensorvalue1;
  calibration2=sensorvalue2;
  calibration3=sensorvalue3;
  calibration4=sensorvalue4+10;
}

void ir_delay (void)
{
    // put your main code here, to run repeatedly:
  qtra.read(IRsensorValues);
for (unsigned char i = 0; i < NUM_SENSORS; i++)
  {
    Serial.print(IRsensorValues[i]);
    Serial.print('\t'); // tab to format the raw data into columns in the Serial monitor
  }
  Serial.println();
  if (IRsensorValues[0] >850)
  {
  motors.setM1Speed(ROBOT_SPEED + ROBOT_ADJUSTMENT);
    motors.setM2Speed(ROBOT_SPEED);
    delay(MOVE_TIME);
  }
  else if (IRsensorValues[1] > 850 && IRsensorValues[2] >850)
  {
  motors.setM1Speed(-(ROBOT_SPEED + ROBOT_ADJUSTMENT));
    motors.setM2Speed(-ROBOT_SPEED);
    delay(700);
          motors.setM1Speed(-(ROBOT_SPEED + ROBOT_ADJUSTMENT));
    motors.setM2Speed(ROBOT_SPEED);

    delay(TURN_TIME);
  }
  else if (IRsensorValues[1] > 850)
  {
      motors.setM1Speed(-(ROBOT_SPEED + ROBOT_ADJUSTMENT));
    motors.setM2Speed(-ROBOT_SPEED);
    delay(600);
      motors.setM1Speed(-(ROBOT_SPEED + ROBOT_ADJUSTMENT));
    motors.setM2Speed(ROBOT_SPEED);

    delay(TURN_TIME);
  }
    else if (IRsensorValues[2] > 850)
  {
      motors.setM1Speed(-(ROBOT_SPEED + ROBOT_ADJUSTMENT));
    motors.setM2Speed(-ROBOT_SPEED);
    delay(600);
  motors.setM1Speed(ROBOT_SPEED + ROBOT_ADJUSTMENT);
    motors.setM2Speed(-ROBOT_SPEED);
    delay(TURN_TIME);
  }
  delay(100);
}

